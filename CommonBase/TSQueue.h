#pragma once
#include "Mutex.h"
namespace Minicat
{
	template<typename T, int nMaxSize = 4096>
	class CTSQueue
	{
	public:
		CTSQueue() :m_nHead(0), m_nTail(0)
		{
		}
		virtual ~CTSQueue()
		{
		}
	private:
		CTSQueue(CTSQueue& other);
		const CTSQueue& operator=(CTSQueue& other);
	public:
		inline bool Push(const T &t)
		{
			CAutoLock autoLock(m_mutex);
			if (IsFull())
			{
				return false;
			}
			m_pElements[m_nTail] = t;
			m_nTail = (m_nTail + 1) % nMaxSize;
			return true;
		}

		inline bool Pop(T &t)
		{
			CAutoLock autoLock(m_mutex);
			if (IsEmpty())
			{
				return false;
			}
			t = m_pElements[m_nHead];
			m_nHead = (m_nHead + 1) % nMaxSize;
			return true;
		}

		inline bool GetHead(T &t)
		{
			CAutoLock autoLock(m_mutex);
			if (IsEmpty())
			{
				return false;
			}
			t = m_pElements[m_nHead];
			return true;
		}

		inline int GetSize()
		{
			CAutoLock autoLock(m_mutex);
			return (m_nTail + nMaxSize - m_nHead) % nMaxSize;
		}

		inline bool IsFull()
		{
			CAutoLock autoLock(m_mutex);
			return ((m_nTail + 1) % nMaxSize == m_nHead);
		}

		inline bool IsEmpty()
		{
			CAutoLock autoLock(m_mutex);
			return (m_nHead == m_nTail);
		}

		inline void Reset()
		{
			CAutoLock autoLock(m_mutex);
			m_nHead = 0;
			m_nTail = 0;
		}

	protected:
		CMutex m_mutex;
		int	m_nHead;
		int	m_nTail;
		T m_pElements[nMaxSize];
	};

}