#pragma once
/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  向量容器类
**********************************************************************************/
#include "CommonBase.h"
namespace Minicat
{
	typedef int(*CompFunc)(const void *pEle1, const void *pEle2);

	template<typename T>
	class CVector
	{
		enum
		{
			Default_Capacity = 10,
		};
	public:
		CVector(int nCapacity = 0) : m_aT(nullptr), m_nCapacity(nCapacity), m_nSize(0)
		{
			if (m_nCapacity == 0)
			{
				m_nCapacity = Default_Capacity;
			}
			m_aT = (T*)malloc(sizeof(T) * m_nCapacity);
		}

		CVector(CVector& other)
		{
			*this = other;
		}

		CVector& operator=(CVector &other)
		{
			m_aT = (T*)malloc(sizeof(T) * other.m_nCapacity);
			memcpy(m_aT, other.m_aT, m_nCapacity * sizeof(T));
			m_nSize = other.m_nSize;
			return *this;
		}

		~CVector()
		{
			for (int i = 0; i < m_nSize; i++)
			{
				m_aT[i].~T();
			}
			SAFE_FREE(m_aT);
		};

		inline int Capacity()
		{
			return m_nCapacity;
		}

		inline int Size()
		{
			return m_nSize;
		}

		bool Reserve(int nCapacity)
		{
			if (m_nCapacity >= nCapacity)
			{
				return true;
			}
			m_nCapacity = nCapacity;
			m_aT = (T*)realloc(m_aT, sizeof(T) * m_nCapacity);
			if (m_aT == nullptr)
			{
				return false;
			}
			return true;
		}

		bool Push(T &t)
		{
			if (m_nSize >= m_nCapacity)
			{
				if (!Reserve(m_nCapacity * 2))
				{
					return false;
				}
			}
			new (&m_aT[m_nSize++]) T(t);
			return true;
		}

		bool Pop(T &t)
		{
			if (m_nSize <= 0)
			{
				return false;
			}
			t = m_aT[--m_nSize];
			return true;
		}

		inline T& operator[] (int nIndex)
		{
			return m_aT[nIndex];
		}

		inline bool Get(int nIndex, T &t)
		{
			if (nIndex < 0 || nIndex >= m_nSize)
			{
				return false;
			}
			t = m_aT[nIndex];
			return true;
		}

		inline bool Set(int nIndex, T &t)
		{
			if (nIndex < 0 || nIndex >= m_nSize)
			{
				return false;
			}
			m_aT[nIndex] = t;
			return true;
		}

		inline void Clear()
		{
			for (int i = 0; i < m_nSize; i++)
			{
				m_aT[i].~T();
			}
			m_nSize = 0;
		}

		void Sort(CompFunc funComp)
		{
			if (m_nSize > 0)
			{
				qsort(m_aT, m_nSize, sizeof(T), funComp);
			}
		}

	private:
		T *m_aT;
		int m_nCapacity;
		int m_nSize;
	};
}


