#pragma once
#include "DataStructure.h"
struct NewPacket
{
	Packet pkt;
	bool isacked;
	NewPacket() : isacked(false){}
	NewPacket(const Packet& pkt) : pkt(pkt), isacked(false){}
};