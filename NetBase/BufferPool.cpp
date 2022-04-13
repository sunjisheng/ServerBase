#include "CommonBase.h"
#include "CommonDefine.h"
#include "BufferPool.h"

namespace Minicat
{
	Block* CBufferPool::m_pBlocks[Max_Block_Size] = {0};
	Block* CBufferPool::m_pCurrent = nullptr;
	int CBufferPool::m_nBlockCount = 0;

	void CBufferPool::Init()
	{
		m_pCurrent = Expand();
	}

	void CBufferPool::Release()
	{
		for (int i = 0; i < Max_Block_Count; i++)
		{
			SAFE_FREE(m_pBlocks[i])
		}
	}

	bool CBufferPool::Alloc(PacketBuf *pBuf)
	{
		if (pBuf->nLen > Max_Block_Size)
		{
			return false;
		}

		//当前块已不够分配
		if (Max_Block_Size - m_pCurrent->nAllocLen < pBuf->nLen)
		{
			m_pCurrent = m_pCurrent->pNext;
		}
		
		if (!m_pCurrent)
		{
			m_pCurrent = Expand();
			if (!m_pCurrent)
			{
				return false;
			}
		}

		pBuf->nBlockNo = m_pCurrent->nBlockNo;
		pBuf->szBuf = (char*)(m_pCurrent + 1) + m_pCurrent->nAllocLen;
		return true;
	}

	bool CBufferPool::Free(PacketBuf *pBuf)
	{
		int nBlockNo = pBuf->nBlockNo;
		int nLen = pBuf->nLen;
		char *szBuf = pBuf->szBuf;

		if (nBlockNo < 0 || nBlockNo >= Max_Block_Count)
		{
			return false;
		}

		if (szBuf < (char*)(m_pBlocks[nBlockNo] + 1) || szBuf >= (char*)(m_pBlocks[nBlockNo] + 1) + Max_Block_Size)
		{
			return false;
		}

		if (m_pBlocks[nBlockNo]->nAllocLen < nLen)
		{
			return false;
		}

		m_pBlocks[nBlockNo]->nAllocLen -= nLen;
		if (m_pBlocks[nBlockNo]->nAllocLen == 0)
		{
			Recycle(m_pBlocks[nBlockNo]);
		}
		return true;
	}

	Block* CBufferPool::Expand()
	{
		if (m_nBlockCount >= Max_Block_Count)
		{
			return nullptr;
		}
		Block *pBlock = (Block *)malloc(sizeof(Block) + Max_Block_Size);
		if (!pBlock)
		{
			return nullptr;
		}
		pBlock->nBlockNo = m_nBlockCount;
		pBlock->nAllocLen = 0;
		pBlock->pNext = nullptr;
		m_pBlocks[m_nBlockCount++] = pBlock;
		return pBlock;
	}

	void CBufferPool::Recycle(Block *pBlock)
	{
		pBlock->pNext = m_pCurrent->pNext;
		m_pCurrent->pNext = pBlock;
	}
}