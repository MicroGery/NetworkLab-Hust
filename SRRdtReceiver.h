#pragma once
#include <unordered_map>
#include <vector>
#include "stdafx.h"
#include "NewPacket.h"
#include "RdtReceiver.h"

class SRRdtReceiver :
	public RdtReceiver
{
private:
	const int WndSize;
	const int Serial;
	int base;
	unordered_map <int, Packet> ReceiverCache;
	vector<Packet> acks;
	bool InWindow(int seqnum);
public:
	SRRdtReceiver(int n, unsigned int seqnum);
	~SRRdtReceiver();
	void receive(const Packet& packet);
};