#include "CommonBase.h"
#include "OutputPackets.h"

namespace Minicat
{
	COutputPackets::COutputPackets() :m_nHead(0), m_nCount(0)
	{

	}

	COutputPackets::~COutputPackets()
	{

	}

	int COutputPackets::GetCount()
	{
		return m_nCount;
	}

	bool COutputPackets::Push(OutPacketBuf *pBuf)
	{
		if (m_nCount >= Max_OutputPackets)
		{
			return false;
		}
		int nPos = (m_nHead + m_nCount) % Max_OutputPackets;
		if (m_arrBuf[nPos].IsValid())
		{
			return false;
		}
		m_arrBuf[nPos] = *pBuf;
		m_nCount++;
		return true;
	}

	bool COutputPackets::Pick(OutPacketBuf *&pBuf)
	{
		if (m_nCount == 0)
		{
			return false;
		}
		pBuf = &m_arrBuf[m_nHead];
		return true;
	}

	bool COutputPackets::Pop()
	{
		if (m_nCount == 0)
		{
			return false;
		}
		m_arrBuf[m_nHead].Reset();
		m_nHead = (m_nHead + 1) % Max_OutputPackets;
		m_nCount--;
		return true;
	}

	void COutputPackets::FreeAll()
	{
		for (int i = 0; i < m_nCount; i++)
		{
			CBufferPool::Free(&m_arrBuf[m_nHead]);
			m_arrBuf[m_nHead].Reset();
			m_nHead = (m_nHead + 1) % Max_OutputPackets;
		}
	}
}