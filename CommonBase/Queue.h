/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  队列模板类
**********************************************************************************/
#pragma once

template<typename T, int nMaxSize = 4096>
class CQueue
{
public:
	CQueue() :m_nHead(0), m_nTail(0)
	{
	}
	virtual ~CQueue()
	{
	}

	inline bool Push(const T &t)
	{
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
		if (IsEmpty())
		{
			return false;
		}
		t = m_pElements[m_nHead];
		return true;
	}

	inline int GetSize()
	{
		return (m_nTail + nMaxSize - m_nHead) % nMaxSize;
	}

	inline bool IsFull()
	{
		return ((m_nTail + 1) % nMaxSize == m_nHead);
	}

	inline bool IsEmpty()
	{
		return (m_nHead == m_nTail);
	}

	inline void Reset()
	{
		m_nHead = 0;
		m_nTail = 0;
	}

protected:
	int	m_nHead;
	int	m_nTail;
	T m_pElements[nMaxSize];
};