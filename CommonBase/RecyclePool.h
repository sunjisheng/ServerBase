/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  固定大小的对象回收池，用于动态分配的对象的重复利用
**********************************************************************************/
#pragma once

namespace Minicat
{
	template<typename T, int iMaxCount>
	class CRecyclePool
	{
	public:
		CRecyclePool()
		{
			m_nPosition = 0;
			m_pElements = new T*[iMaxCount];
			memset(m_pElements, 0, sizeof(T*) * iMaxCount);
		}

		~CRecyclePool()
		{
			for (int i = 0; i < m_nPosition; i++)
			{
				delete m_pElements[i];
			}
			m_nPosition = 0;
			delete[] m_pElements;
		}
	private:
		CRecyclePool(CRecyclePool &other);
		const CRecyclePool& operator=(CRecyclePool &other);
	public:

		inline bool Free(T* t)
		{
			if (t != nullptr && m_nPosition < iMaxCount)
			{
				m_pElements[m_nPosition++] = t;
				return true;
			}
			return false;
		}

		inline T* Alloc()
		{
			if (m_nPosition > 0)
			{
				return m_pElements[--m_nPosition];
			}
			return nullptr;
		}

		inline int GetCount() const
		{
			return m_nPosition;
		}

	private:
		int m_nPosition;
		T** m_pElements;
	};

}
	