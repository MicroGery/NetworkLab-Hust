#pragma once
#include "RdtSender.h"
#include "DataStructure.h"
#include "NewPacket.h"
#include <vector>
#include <iostream>
using namespace std;
void keep_not_open(vector<int>& r);
class SRRdtSender :
	public RdtSender
{
private :
	const int WndSize;
	const int Serial;
	int base;
	int NextSeqNum;
	vector<NewPacket> SenderCache;
	bool InWindow(int acknum);
public :
	bool getWaitingState();
	bool send(const Message& message);
	void receive(const Packet& packet);
	void timeoutHandler(int seqNum);

	SRRdtSender();
	SRRdtSender(int wsize, int serial);
	~SRRdtSender();
};