#include "GBNRdtSender.h"
#include "stdafx.h"
#include "Global.h"
#include "makepkt.h"
GBNRdtSender::GBNRdtSender() :
	WndSize(4),
	Serial(8),
	base(0),
	NextSeqNum(0),
	packects(vector<Packet>(8)) {}

GBNRdtSender::GBNRdtSender(int wndsize, int serial) :
	Serial(serial > 0 ? 1 << serial : 1 << 16),
	WndSize(wndsize),
	base(0),
	NextSeqNum(0),
	packects(vector<Packet>(8)) {}

GBNRdtSender::~GBNRdtSender() {}

bool GBNRdtSender::getWaitingState() {
	return NextSeqNum == (base + WndSize) % Serial;
}

bool GBNRdtSender::send(const Message& message) {
	if (getWaitingState()) {
		std::cout << "Sender 窗口已满，请稍后再试\n";
		return false;
	}
	else {
		Packet pkt = make_data_pkt(NextSeqNum, message.data);
		packects[NextSeqNum % WndSize] = pkt;
		pUtils->printPacket("发送方发送Packet", pkt);
		pns->sendToNetworkLayer(RECEIVER, pkt);
		if (base == NextSeqNum)
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		NextSeqNum = (NextSeqNum + 1) % Serial;
		return true;
	}
}
void GBNRdtSender::receive(const Packet &ackpacket) {
	int checksum = pUtils->calculateCheckSum(ackpacket);
	if (checksum == ackpacket.checksum) {
		base = (ackpacket.acknum + 1) % Serial;
		pUtils->printPacket("发送方正确收到ACK-Packet", ackpacket);
		pns->stopTimer(SENDER, 0);
		cout << "窗口变化:[" << base << "," << (base + WndSize) % Serial << "]" << endl;
		if (base != NextSeqNum)
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	}
	else
		pUtils->printPacket("发送方接收到损坏ACK-Packet", ackpacket);
}

void GBNRdtSender::timeoutHandler(int serial) {
	cout << "超时！重发报文" << endl;
	pns->stopTimer(SENDER, 0);
	cout << base << " " << NextSeqNum << endl;
	for (int i = base; i != NextSeqNum; i=(i+1)%Serial) {
		pns->sendToNetworkLayer(RECEIVER, packects[i % WndSize]);
		pUtils->printPacket("重发报文", packects[i % WndSize]);
	}
	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
}

