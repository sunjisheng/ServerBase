#include "InputPackets.h"
#include "UdpManager.h"
namespace Minicat
{
	CIuputPackets::CIuputPackets() :m_nHead(0), m_nDiff(0)
	{

	}

	CIuputPackets::~CIuputPackets()
	{

	}

	bool CIuputPackets::Push(int nDiff, PacketBuf *pBuf)
	{
		if (nDiff < 0 || nDiff >= Max_InputPackets)
		{
			return false;
		}

		int nPos = (m_nHead + nDiff) % Max_InputPackets;

		if (m_arrBuf[nPos].IsValid())
		{
			return false;
		}

		m_arrBuf[nPos].nBlockNo = pBuf->nBlockNo;
		m_arrBuf[nPos].szBuf = pBuf->szBuf;
		m_arrBuf[nPos].nLen = pBuf->nLen;
		
		if (nDiff > m_nDiff)
		{
			m_nDiff = nDiff;
		}
		return true;
	}

	bool CIuputPackets::Pop(PacketBuf *pBuf)
	{
		if (!m_arrBuf[m_nHead].IsValid())
		{
			return false;
		}
		pBuf->nBlockNo = m_arrBuf[m_nHead].nBlockNo;
		pBuf->szBuf = m_arrBuf[m_nHead].szBuf;
		pBuf->nLen = m_arrBuf[m_nHead].nLen;
		m_arrBuf[m_nHead].Reset();
		m_nHead = (m_nHead + 1) % Max_InputPackets;
		m_nDiff = m_nDiff - 1;
		return true;
	}

	void CIuputPackets::FreeAll()
	{
		for (int nDiff = 0; nDiff < m_nDiff; nDiff++)
		{
			int nPos = (m_nHead + nDiff) % Max_InputPackets;
			if (m_arrBuf[nPos].IsValid())
			{
				CBufferPool::Free(&m_arrBuf[nPos]);
				m_arrBuf[nPos].Reset();
			}
		}
	}
}