#pragma once
#include "stdafx.h"
#include "Global.h"
#include "DataStructure.h"

Packet make_pkt(int seqNum, int ackNum,const char data[Configuration::PAYLOAD_SIZE]);
Packet make_data_pkt(int seqNum,const char data[Configuration::PAYLOAD_SIZE]);
Packet make_ack_pkt(int ackNum);