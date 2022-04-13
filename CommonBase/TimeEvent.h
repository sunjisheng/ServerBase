/*********************************************************************************
  *����:  SunJiSheng
  *����:  2020/04/15
  *����:  ��ʱ������,��CTimeScheduler����
**********************************************************************************/
#pragma once

namespace Minicat
{
	template <typename T>
	class TimeEvent
	{
	public:
		typedef void(T::*TimerCallback)();

		TimeEvent() :pOwner(nullptr), nFrame(-1), pPrev(nullptr), pNext(nullptr)
		{
			fnOnTimer = nullptr;
		}

		bool IsScheduling()
		{
			return (nFrame >= 0);
		}

		void Reset()
		{
			nFrame = -1;
			pPrev = nullptr;
			pNext = nullptr;
		}
		T *pOwner;
		TimerCallback fnOnTimer;
		TimeEvent *pPrev;
		TimeEvent *pNext;
		int nFrame;
	};
}

