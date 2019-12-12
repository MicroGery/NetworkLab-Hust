#include "makepkt.h"
Packet make_pkt(int seqNum, int ackNum,const char data[Configuration::PAYLOAD_SIZE]) {
	Packet pkt;
	pkt.acknum = ackNum;
	pkt.seqnum = seqNum;
	memcpy(pkt.payload, data, Configuration::PAYLOAD_SIZE);
	pkt.checksum = pUtils->calculateCheckSum(pkt);
	return pkt;
}

Packet make_data_pkt(int seqNum,const char data[Configuration::PAYLOAD_SIZE]) {
	return make_pkt(seqNum, -1, data);
}

Packet make_ack_pkt(int ackNum) {
	char data[Configuration::PAYLOAD_SIZE] = "ACK";
	return make_pkt(-1, ackNum, data);
}