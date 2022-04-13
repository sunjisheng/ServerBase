/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  三指针的可滑动输出缓冲区
**********************************************************************************/
#pragma once
#include "CommonBase.h"
namespace Minicat
{
	class COutSlideQueue
	{
	public:
		COutSlideQueue() :m_nHead(0), m_nMid(0), m_nTail(0), m_nMaxSize(0)
		{
		};

		~COutSlideQueue()
		{
			SAFE_FREE(m_pBuffer);
		};

	public:

		inline void Init(int nMaxSize)
		{
			m_nMaxSize = nMaxSize;
			m_pBuffer = (char*)malloc(m_nMaxSize);
		}

		inline void Reset()
		{
			m_nHead = 0;
			m_nMid = 0;
			m_nTail = 0;
		}

		inline int GetMaxSize()
		{
			return m_nMaxSize;
		}

		inline bool IsEmpty()
		{
			return (m_nHead == m_nTail);
		}

		inline bool IsReadEmpty()
		{
			return (m_nMid == m_nTail);
		}

		inline bool IsConfirmEmpty()
		{
			return (m_nHead == m_nMid);
		}

		inline int GetReadSize()
		{
			return (m_nTail + m_nMaxSize - m_nMid) % m_nMaxSize;
		}

		inline int GetConfirmSize()
		{
			return (m_nMid + m_nMaxSize - m_nHead) % m_nMaxSize;
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

		inline bool ReadPeek(char *szBuf, int nSize)
		{
			if (GetReadSize() < nSize)
			{
				return false;
			}

			int nToEndSize = m_nMaxSize - m_nMid;
			if (nToEndSize >= nSize)
			{
				memcpy(szBuf, &m_pBuffer[m_nMid], nSize);
			}
			else
			{
				memcpy(szBuf, &m_pBuffer[m_nMid], nToEndSize);
				memcpy(&szBuf[nToEndSize], &m_pBuffer[0], nSize - nToEndSize);
			}
			return true;
		}

		inline bool Read(char *szBuf, int nSize)
		{
			if (!ReadPeek(szBuf, nSize))
			{
				return false;
			}
			m_nMid = (m_nMid + nSize) % m_nMaxSize;
			return true;
		}

		inline char* GetWritePtr()
		{
			return &m_pBuffer[m_nTail];
		}

		inline char* GetReadPtr()
		{
			return &m_pBuffer[m_nMid];
		}

		inline char* GetConrimPtr()
		{
			return &m_pBuffer[m_nHead];
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

		inline int GetReadLen()
		{
			if (m_nMid > m_nTail)
				return m_nMaxSize - m_nMid;
			else
				return m_nTail - m_nMid;
		}

		inline int GetConfirmLen()
		{
			return (m_nMid + m_nMaxSize - m_nHead) % m_nMaxSize;
		}

		inline void WriteSkip(int nLen)
		{
			m_nTail = (m_nTail + nLen) % m_nMaxSize;
		}

		inline void ReadSkip(int nLen)
		{
			m_nMid = (m_nMid + nLen) % m_nMaxSize;
		}

		inline void ConfirmSkip(int nLen)
		{
			m_nHead = (m_nHead + nLen) % m_nMaxSize;
		}

		inline bool IsCircleBack(char *szBuf, int nLen)
		{
			return (m_nMaxSize - (szBuf - m_pBuffer) < nLen);
		}

	protected:
		int m_nMaxSize;
		char* m_pBuffer;
		int	m_nHead;
		int m_nMid;
		int	m_nTail;
	};
}