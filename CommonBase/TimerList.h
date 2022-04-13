#pragma once
#include "Timer.h"
#include "Map.h"
namespace Minicat
{
	class CTimerList
	{
	public:
		CTimerList() {};
		virtual ~CTimerList() {};
		virtual void OnTimer(int nTimerID) {};
	public:
		bool SetTimer(int nTimerID, int nMilliSec);
		bool KillTimer(int nTimerID);
		void Tick();
	protected:
		CMap<int,CTimer>	m_mapTimer;
	};
}