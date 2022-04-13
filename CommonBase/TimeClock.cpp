#include "CommonBase.h"
#include "TimeClock.h"
#include "GlobalFunction.h"
namespace Minicat
{
	uint64 CTimeClock::m_nTick = 0;
	time_t CTimeClock::m_nTime = 0;
	tm CTimeClock::m_tm = {0};

	void CTimeClock::Update()
	{
		m_nTick = GetTickCount();
	}

	uint64 CTimeClock::GetTick()
	{
		return m_nTick;
	}

	time_t CTimeClock::GetTime()
	{
		::time(&m_nTime);
		Convert2TM(&m_nTime, &m_tm);
		return m_nTime;
	}

	void CTimeClock::Convert2TM(const time_t *_tmt, tm *_tm)
	{
#if defined(_WINDOWS)
		localtime_s(_tm, _tmt);
#elif defined(_LINUX)
		localtime_r(_tmt, _tm);
#endif
	}
}
