#include "pch.h"
#include "Logger.h"
#include "GlobalFunction.h"
#include "TimeClock.h"
#include "Random.h"
using namespace Minicat;
void testlog_main()
{
	CLogger::Instance()->StartLog("test");
	CRandom::SRand();
	while (true)
	{
		CTimeClock::Update();
		int nCount = CRandom::Rand(10);
		for (int i = 0; i < nCount; i++)
		{
			WriteLog(Log_Level_Error, "%d this is test log message, he is bigman, please sit down, helloworld", i);
		}
		MySleep(50);
	}
}

