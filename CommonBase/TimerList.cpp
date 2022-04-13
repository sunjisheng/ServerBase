#include "CommonBase.h"
#include "TimerList.h"

namespace Minicat
{
	bool CTimerList::SetTimer(int nTimerID, int nMilliSec)
	{
		CTimer timer;
		timer.SetTimerID(nTimerID);
		timer.StartTimer(nMilliSec);
		m_mapTimer.Set(nTimerID, timer);
		return true;
	}

	bool CTimerList::KillTimer(int nTimerID)
	{
		return m_mapTimer.Erase(nTimerID);
	}

	void CTimerList::Tick()
	{
		CMap<int, CTimer>::TreeNode *pNode = m_mapTimer.Begin();;
		while (pNode != NULL)
		{
			if (pNode->m_Value.CountingTimer())
			{
				OnTimer(pNode->m_Key);
				pNode = m_mapTimer.Erase(pNode);
			}
			else
			{
				pNode = pNode->Next();
			}
		}
	}
}
