/*********************************************************************************
  *����:  SunJiSheng
  *����:  2020/04/15
  *����:  ���󹤳����ڲ�����һ�����У���������Ӧ��ԼΪ���Socket����
			��������֮ǰ�ͷŵĶ��󣬵�Ҳ��֤�ոջ��յĶ��󲻻ᱻ��������
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
