#include "GBNRdtReceiver.h"
#include "Global.h"
#include "makepkt.h"
#include "stdafx.h"

GBNRdtReceiver::GBNRdtReceiver(unsigned int seqnum) :
	Serial(seqnum > 1 ? (1 << seqnum) : (1 << 16)),
	ExpectedSeqnum(0),
	LastAckPkt(make_ack_pkt(-1)) {}

GBNRdtReceiver::~GBNRdtReceiver() {}

void GBNRdtReceiver::receive(const Packet & packet) {
	int checksum = pUtils->calculateCheckSum(packet);
	if (checksum == packet.checksum && ExpectedSeqnum == packet.seqnum) {
		pUtils->printPacket("接收方接收到正确报文", packet);
		LastAckPkt = make_ack_pkt(packet.seqnum);
		Message msg;
		memcpy(msg.data, packet.payload, sizeof(packet.payload));
		pns->delivertoAppLayer(RECEIVER, msg);
		pUtils->printPacket("接收方发送ACK报文", LastAckPkt);
		ExpectedSeqnum = (ExpectedSeqnum + 1) % Serial;
	}
	else {
		if (checksum != packet.checksum) {
			pUtils->printPacket("接收方接收到损坏报文", packet);
		}
		else if (packet.seqnum != ExpectedSeqnum) {
			pUtils->printPacket("报文失序", packet);
		}
		pUtils->printPacket("接收方重发上一个ACK报文", LastAckPkt);
	}
	pns->sendToNetworkLayer(SENDER, LastAckPkt);
}