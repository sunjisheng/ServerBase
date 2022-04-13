#pragma once
#include "CommonBase.h"
#include "GlobalFunction.h"
namespace Minicat
{
	class CFps
	{
	public:
		CFps() : m_nInterval(0)
		{
			Reset();
		}
	public:
		int m_nFps;
		int	m_nInterval;
		unsigned long long m_nExpected;
	public:
		void SetFps(int fps)
		{
			m_nFps = fps;
			m_nInterval = int(1000 / fps);
			Reset();
		}
		inline void Tick()
		{
			uint64 nNow = CTimeClock::GetTick();
			int	nDiff = (int)(m_nExpected - nNow);
			if (nDiff > 0)
			{
				// 休息这么长时间
				MySleep(nDiff);
			}
			m_nExpected += m_nInterval;
		}

		void Reset()
		{
			m_nExpected = GetTickCount() + m_nInterval;
		}
	};
}
