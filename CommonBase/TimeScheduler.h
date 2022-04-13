/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  基于时间轮转模式实现的定时器调度类，效率高于排序和轮询方法
**********************************************************************************/
#pragma once
#include "TimeEvent.h"
#include "Singleton.h"
namespace Minicat
{
	enum
	{
		TimeFrame_Total = 6000,
	};

	template<typename T>
	class TimeFrame
	{
	public:
		TimeFrame() : nFrame(-1), pHead(nullptr), pTail(nullptr)
		{
		}

		void Insert(TimeEvent<T> *pEvent)
		{
			pEvent->nFrame = nFrame;
			if (!pHead)
			{
				pHead = pEvent;
				pTail = pEvent;
			}
			else
			{
				pTail->pNext = pEvent;
				pEvent->pPrev = pTail;
				pTail = pEvent;
			}
		}

		void Remove(TimeEvent<T> *pEvent)
		{
			if (pEvent->nFrame != nFrame)
			{
				return;
			}
			if (pEvent->pPrev)
			{
				pEvent->pPrev->pNext = pEvent->pNext;
			}
			else
			{
				pHead = pEvent->pNext;
			}

			if (pEvent->pNext)
			{
				pEvent->pNext->pPrev = pEvent->pPrev;
			}
			else
			{
				pTail = pEvent->pPrev;
			}
			pEvent->Reset();
		}

		void Fire()
		{
			TimeEvent<T> *pEvent = pHead;
			TimeEvent<T> *pNext = nullptr;
			while (pEvent != nullptr)
			{
				if (pEvent->fnOnTimer)
				{
					(pEvent->pOwner->*pEvent->fnOnTimer)();
				}
				pNext = pEvent->pNext;
				pEvent->Reset();
				pEvent = pNext;
			}
			pHead = nullptr;
			pTail = nullptr;
		}
	public:
		TimeEvent<T> *pHead;
		TimeEvent<T> *pTail;
		int nFrame;
	};

	template<typename T>
	class CTimeScheduler : public Singleton<CTimeScheduler<T>>
	{
	public:
		void Init(int nFrameCount = TimeFrame_Total)
		{
			m_nFrameCount = nFrameCount;
			m_nCurFrame = 0;
			m_pTimeFrames = new TimeFrame<T>[nFrameCount]();
			for (int nFrame = 0; nFrame < nFrameCount; nFrame++)
			{
				m_pTimeFrames[nFrame].nFrame = nFrame;
			}
		}

		void AddTimer(int nFrame, TimeEvent<T>* pEvent)
		{
			nFrame = (m_nCurFrame + nFrame) % TimeFrame_Total;
			m_pTimeFrames[nFrame].Insert(pEvent);
		}

		void DelTimer(TimeEvent<T> *pEvent)
		{
			m_pTimeFrames[pEvent->nFrame].Remove(pEvent);
		}

		void Tick()
		{
			m_pTimeFrames[m_nCurFrame].Fire();
			m_nCurFrame = (m_nCurFrame + 1) % m_nFrameCount;
		}
	protected:
		TimeFrame<T>* m_pTimeFrames;
		int m_nFrameCount;
		int m_nCurFrame;
	};
}
