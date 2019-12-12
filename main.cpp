// RdtLab.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "stdafx.h"
#include "DataStructure.h"
#include "GBNRdtReceiver.h"
#include "GBNRdtSender.h"
#include "Global.h"
#include "makepkt.h"
#include "NetworkService.h"
#include "RandomEventEnum.h"
#include "RdtReceiver.h"
#include "RdtSender.h"
#include "targetver.h"
#include "Tool.h"
#include "SRRdtReceiver.h"
#include "SRRdtSender.h"
int main()
{
	RdtSender* ps = new SRRdtSender(4,3);
	RdtReceiver* pr = new SRRdtReceiver(4,3);
	pns->setRunMode(1);  //安静模式
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("E:\\Network Lab\\RdtLab\\RdtLab\\input.txt");
	pns->setOutputFile("E:\\Network Lab\\RdtLab\\RdtLab\\output.txt");
	pns->start();
	delete ps;
	delete pr;
	delete pUtils;                                  //指向唯一的工具类实例，只在main函数结束前delete
	delete pns;                                     //指向唯一的模拟网络环境类实例，只在main函数结束前delete
	return 0;
}
