#include "CommonBase.h"
#include "Timer.h"
#include "TimeClock.h"
namespace Minicat
{
	CTimer::CTimer(void)
	{
		m_nTimerID = 0;
		m_nInterval = 0;
		m_nElapseTime = 0;
		m_nLastTick = 0;
		m_bPause = true;
	}

	CTimer::~CTimer(void)
	{
	}

	void CTimer::StartTimer(int nInterval)
	{
		m_nLastTick = CTimeClock::GetTick();
		m_nElapseTime = 0;
		m_nInterval = nInterval;
		m_bPause = false;
	}

	void CTimer::Pause(bool bPause)
	{
		if (m_bPause == bPause)
			return;
		m_bPause = bPause;
		uint64 lNow = CTimeClock::GetTick();
		if (bPause)
		{
			m_nElapseTime += lNow - m_nLastTick;
		}
		else
		{
			m_nLastTick = lNow;
		}
	}

	bool CTimer::CountingTimer()
	{
		if (m_bPause)
		{
			return false;
		}

		uint64 lNow = CTimeClock::GetTick();
		m_nElapseTime += lNow - m_nLastTick;
		m_nLastTick = lNow;

		if (m_nElapseTime < m_nInterval)
			return false;
		//开始下一次定时
		m_nElapseTime = m_nElapseTime - m_nInterval;
		return true;
	}
}