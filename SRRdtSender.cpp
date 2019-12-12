#include "stdafx.h"
#include "SRRdtSender.h"
#include "Global.h"
#include "makepkt.h"

SRRdtSender::SRRdtSender() :
	WndSize(4),
	Serial(8),
	base(0),
	NextSeqNum(0),
	SenderCache(vector<NewPacket>(8)){}
	
SRRdtSender::SRRdtSender(int wndsize, int serial) :
	Serial(serial > 0 ? 1 << serial : 1 << 16),
	WndSize(wndsize),
	base(0),
	NextSeqNum(0),
	SenderCache(vector<NewPacket>(serial > 0 ? 1 << serial : 1 << 16)){}

SRRdtSender::~SRRdtSender(){}
bool SRRdtSender::getWaitingState() {
	return NextSeqNum == (base + WndSize) % Serial;
}

bool SRRdtSender::send(const Message& message) {
	if (getWaitingState()) {
		cout << "发送窗口满，请稍后再试" << endl;
		return false;
	}
	else {
		Packet SendPacket = make_data_pkt(NextSeqNum, message.data);
		SenderCache[NextSeqNum % WndSize] = SendPacket;
		SenderCache[NextSeqNum % Serial] = SendPacket;
		pUtils->printPacket("发送方发送数据报文", SendPacket);
		pns->sendToNetworkLayer(RECEIVER, SendPacket);
		pns->startTimer(SENDER, Configuration::TIME_OUT, NextSeqNum);
		NextSeqNum = (NextSeqNum + 1) % Serial;
		return true;
	}
}

bool SRRdtSender::InWindow(int acknum) {
	if (base < (base + WndSize) % Serial)
		return acknum >= base && acknum < (base + WndSize) % Serial;
	else
		return acknum >= base || acknum < (base + WndSize) % Serial;
}

void SRRdtSender::receive(const Packet& ackpacket) {
	int checksum = pUtils->calculateCheckSum(ackpacket);
	if (checksum == ackpacket.checksum && InWindow(ackpacket.acknum)) {
		SenderCache[ackpacket.acknum % WndSize].isacked = true;
		pUtils->printPacket("发送方正确接收到ACK数据报", ackpacket);
		pns->stopTimer(SENDER, ackpacket.acknum);
		if (ackpacket.acknum == base) {
			while (base != NextSeqNum && SenderCache[base % WndSize].isacked) {
				base = (base + 1) % Serial;
			}
		}
		cout << "发送方窗口变化为[" << base << "," << (base + WndSize) % Serial << "]" << endl;
	}
	else
		pUtils->printPacket("发送方未能正确接收ACK数据报", ackpacket);
}

void SRRdtSender::timeoutHandler(int seqnum) {
	pns->stopTimer(SENDER, seqnum);
	pns->sendToNetworkLayer(RECEIVER, SenderCache[seqnum % WndSize].pkt);
	pUtils->printPacket("超时！重发报文", SenderCache[seqnum % WndSize].pkt);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqnum);
}

