#pragma once
#include "RdtSender.h"
#include "DataStructure.h"
#include <vector>
#include <iostream>
using namespace std;
void keep_not_open(vector<int>& r);
class GBNRdtSender : 
	public RdtSender
{
public:
	bool getWaitingState();
	bool send(const Message& message);
	void receive(const Packet& packet);
	void timeoutHandler(int seqNum);
	
	GBNRdtSender();
	GBNRdtSender(int wsize, int sSize);
	virtual ~GBNRdtSender();


private:
	int base;						//最早的未确认的序号
	int NextSeqNum;					//下一个待发分组的序号
	const int WndSize;				//窗口大小
	const int Serial;				//报文序号二进制位数
	std::vector<Packet> packects;	//已发送且为确认的Packets
};
