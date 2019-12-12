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
	int base;						//�����δȷ�ϵ����
	int NextSeqNum;					//��һ��������������
	const int WndSize;				//���ڴ�С
	const int Serial;				//������Ŷ�����λ��
	std::vector<Packet> packects;	//�ѷ�����Ϊȷ�ϵ�Packets
};
