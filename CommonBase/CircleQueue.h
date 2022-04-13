/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  环形缓冲区，用于两个线程间的数据通信
**********************************************************************************/
#pragma once
#include "CommonBase.h"
namespace Minicat
{
	class CCircleQueue
	{
	public:
		CCircleQueue() :m_nHead(0), m_nTail(0), m_nMaxSize(0)
		{
		};

		~CCircleQueue() 
		{
			SAFE_FREE(m_pBuffer);
		};
	private:
		CCircleQueue(CCircleQueue &other)
		{
			*this = other;
		}

		CCircleQueue& operator=(CCircleQueue &other)
		{
			m_nMaxSize = other.m_nMaxSize;
			m_pBuffer = (char*)malloc(m_nMaxSize);
			memcpy(m_pBuffer, other.m_pBuffer, m_nMaxSize);
			m_nHead = other.m_nHead;
			m_nTail = other.m_nTail;
			return *this;
		}
	public:

		inline void Init(int nMaxSize)
		{
			m_nMaxSize = nMaxSize;
			m_pBuffer = (char*)malloc(m_nMaxSize);
		}

		inline void Attach(char *szBuf, int nSize)
		{
			m_pBuffer = szBuf;
			m_nMaxSize = nSize;
		}

		inline void Detach()
		{
			m_pBuffer = NULL;
			m_nMaxSize = 0;
			m_nHead = 0;
			m_nTail = 0;
		}

		inline void Reset()
		{
			m_nHead = 0;
			m_nTail = 0;
		}
		
		inline bool IsEmpty()
		{
			return (m_nHead == m_nTail);
		}

		inline int GetSize()
		{
			return (m_nTail + m_nMaxSize - m_nHead) % m_nMaxSize;
		}

		inline int GetHead()
		{
			return m_nHead;
		}

		inline int GetTail()
		{
			return m_nTail;
		}

		inline int GetSpace()
		{
			return (m_nHead + m_nMaxSize - m_nTail - 1) % m_nMaxSize;
		}

		inline bool Write(const char *szBuf, int nSize)
		{
			if (GetSpace() < nSize)
			{
				return false;
			}

			int nToEndSize = m_nMaxSize - m_nTail;
			if (nToEndSize >= nSize)
			{
				memcpy(&m_pBuffer[m_nTail], szBuf, nSize);
			}
			else
			{
				memcpy(&m_pBuffer[m_nTail], szBuf, nToEndSize);
				memcpy(&m_pBuffer[0], &szBuf[nToEndSize], nSize - nToEndSize);
			}

			m_nTail = (m_nTail + nSize) % m_nMaxSize;
			return true;
		}

		inline bool Peek(char *szBuf, int nSize)
		{
			if (GetSize() < nSize)
			{
				return false;
			}

			int nToEndSize = m_nMaxSize - m_nHead;
			if (nToEndSize >= nSize)
			{
				memcpy(szBuf, &m_pBuffer[m_nHead], nSize);
			}
			else
			{
				memcpy(szBuf, &m_pBuffer[m_nHead], nToEndSize);
				memcpy(&szBuf[nToEndSize], &m_pBuffer[0], nSize - nToEndSize);
			}
			return true;
		}

		inline bool Read(char *szBuf, int nSize)
		{
			if (!Peek(szBuf, nSize))
			{
				return false;
			}
			m_nHead = (m_nHead + nSize) % m_nMaxSize;
			return true;
		}

		inline void Skip(int nSize)
		{
			m_nHead = (m_nHead + nSize) % m_nMaxSize;
		}

		inline char* GetWritePtr()
		{
			return &m_pBuffer[m_nTail];
		}

		inline int GetWriteLen()
		{
			int nToEndSize = m_nMaxSize - m_nTail;
			if (nToEndSize > GetSpace())
			{
				nToEndSize = GetSpace();
			}
			return nToEndSize;
		}

		inline char* GetReadPtr()
		{
			return &m_pBuffer[m_nHead];
		}

		inline int GetReadLen()
		{
			if (m_nHead > m_nTail)
				return m_nMaxSize - m_nHead;
			else
				return m_nTail - m_nHead;
		}

		inline void OnWrited(int nLen)
		{
			m_nTail = (m_nTail + nLen) % m_nMaxSize;
		}

		inline void OnReaded(int nLen)
		{
			m_nHead = (m_nHead + nLen) % m_nMaxSize;
		}

	private:
		int m_nMaxSize;
		char* m_pBuffer;
		int	m_nHead;
		int	m_nTail;
	};
}