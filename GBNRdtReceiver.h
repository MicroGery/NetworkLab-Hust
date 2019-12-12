#pragma once
#include "stdafx.h"
#include "DataStructure.h"
#include "RdtReceiver.h"
#include "makepkt.h"
class GBNRdtReceiver : 
	public RdtReceiver
{
public:
	GBNRdtReceiver(unsigned int seqnum);
	~GBNRdtReceiver();
public:
	void receive(const Packet &packet);

private:
	int ExpectedSeqnum;
	Packet LastAckPkt;
	const int Serial;
	
};
