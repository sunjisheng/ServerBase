/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  对象工厂，内部包含一个队列，队列数量应该约为最大Socket数量
			尽量重用之前释放的对象，但也保证刚刚回收的对象不会被立即重用
**********************************************************************************/

#pragma once
#include "Queue.h"
#include "TimeClock.h"
#include "Singleton.h"

namespace Minicat
{
	enum
	{
		Max_Reusable_Count = 8192,
	};
	
	
	template <typename T>
	class CSocketFactory : public Singleton<CSocketFactory<T>>
	{
		struct CReusableSocket
		{
			uint nRecycleTime;
			T* pSocket;
		};
	public:
		CSocketFactory()
		{
		}
		~CSocketFactory()
		{
			CReusableSocket reusableSocket;
			while (m_queueReusableSocket.Pop(reusableSocket))
			{
				SAFE_DELETE(reusableSocket.pSocket);
			}
		}
	public:
		T* Alloc()
		{
			CReusableSocket reusableSocket;
			if (m_queueReusableSocket.Pop(reusableSocket))
			{
				time_t now = CTimeClock::GetTime();
				if (now - reusableSocket.nRecycleTime > 2)
				{
					reusableSocket.pSocket->Reset();
					return reusableSocket.pSocket;
				}
			}
			return new T();
		}

		bool Free(T *pSocket)
		{
			CReusableSocket reusableSocket;
			reusableSocket.nRecycleTime = (uint)CTimeClock::GetTime();
			reusableSocket.pSocket = pSocket;
			if(!m_queueReusableSocket.Push(reusableSocket))
			{
				SAFE_DELETE(pSocket);
			}
			return true;
		}
		
	private:
		CQueue<CSocketFactory<T>::CReusableSocket, Max_Reusable_Count> m_queueReusableSocket;
	};
}
