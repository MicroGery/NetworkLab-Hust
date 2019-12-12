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
		cout << "���ʹ����������Ժ�����" << endl;
		return false;
	}
	else {
		Packet SendPacket = make_data_pkt(NextSeqNum, message.data);
		SenderCache[NextSeqNum % WndSize] = SendPacket;
		SenderCache[NextSeqNum % Serial] = SendPacket;
		pUtils->printPacket("���ͷ��������ݱ���", SendPacket);
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
		pUtils->printPacket("���ͷ���ȷ���յ�ACK���ݱ�", ackpacket);
		pns->stopTimer(SENDER, ackpacket.acknum);
		if (ackpacket.acknum == base) {
			while (base != NextSeqNum && SenderCache[base % WndSize].isacked) {
				base = (base + 1) % Serial;
			}
		}
		cout << "���ͷ����ڱ仯Ϊ[" << base << "," << (base + WndSize) % Serial << "]" << endl;
	}
	else
		pUtils->printPacket("���ͷ�δ����ȷ����ACK���ݱ�", ackpacket);
}

void SRRdtSender::timeoutHandler(int seqnum) {
	pns->stopTimer(SENDER, seqnum);
	pns->sendToNetworkLayer(RECEIVER, SenderCache[seqnum % WndSize].pkt);
	pUtils->printPacket("��ʱ���ط�����", SenderCache[seqnum % WndSize].pkt);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqnum);
}

