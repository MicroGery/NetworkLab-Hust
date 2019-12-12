#include "SRRdtReceiver.h"
#include "Global.h"
#include "makepkt.h"
#include "stdafx.h"

SRRdtReceiver::SRRdtReceiver(int wndsize, unsigned int serial) :
	Serial(serial > 0 ? 1 << serial : 1 << 16),
	WndSize(wndsize),
	base(0),
	acks(vector<Packet>(wndsize)){}

SRRdtReceiver::~SRRdtReceiver(){}

bool SRRdtReceiver::InWindow(int acknum) {
	if (base < (base + WndSize) % Serial)
		return acknum >= base && acknum < (base + WndSize) % Serial;
	else
		return acknum >= base || acknum < (base + WndSize) % Serial;
}

void SRRdtReceiver::receive(const Packet& pkt) {
	int checksum = pUtils->calculateCheckSum(pkt);
	if (checksum == pkt.checksum && InWindow(pkt.seqnum)) {
		pUtils->printPacket("接收方收到正确报文", pkt);
		if (ReceiverCache.find(pkt.seqnum) == ReceiverCache.end())
			ReceiverCache[pkt.seqnum] = pkt;
		if (pkt.seqnum == base) {
			unordered_map<int, Packet>::const_iterator iter;
			while (!ReceiverCache.empty() && (iter = ReceiverCache.find(base)) != ReceiverCache.end()) {
				Message msg;
				memcpy(msg.data, iter->second.payload, Configuration::PAYLOAD_SIZE);
				pns->delivertoAppLayer(RECEIVER, msg);
				ReceiverCache.erase(iter);
				base = (base + 1) % Serial;
			}
		}
		Packet ackpkt = make_ack_pkt(pkt.seqnum);
		acks[pkt.seqnum % WndSize] = ackpkt;
		pUtils->printPacket("接收方发送ACK报文", ackpkt);
	}
	else if (checksum == pkt.checksum && (InWindow(pkt.seqnum) == false)) {
		pUtils->printPacket("报文序号错误", pkt);
		pUtils->printPacket("重新发送上一个ACK报文", acks[pkt.seqnum % WndSize]);
		pns->sendToNetworkLayer(SENDER, acks[pkt.seqnum % WndSize]);
	}
	else
		pUtils->printPacket("接收方收到损坏报文", pkt);
}