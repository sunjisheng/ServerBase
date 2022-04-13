/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  定长数组
**********************************************************************************/

#pragma once
namespace Minicat
{
	template<typename T>
	class CArray
	{
	public:
		CArray(int nSize) : m_aT(nullptr)
		{
			m_aT = new T[nSize];
			m_nSize = nSize;
		}

		~CArray()
		{
			delete[] m_aT;
		};
	
		CArray(CArray& other)
		{
			*this = other;
		}

		CArray& operator=(CArray &other)
		{
			m_aT = new T[other.m_nSize];
			m_nSize = other.nSize;
			memcpy(m_aT, other.m_aT, m_nSize * sizeof(T));
			return *this;
		}

		inline int Size()
		{
			return m_nSize;
		}

		inline bool Push(T &t)
		{

		}

		inline void Set(int nIndex, T&t)
		{
			if (nIndex < 0 || nIndex >= m_nSize)
			{
				return;
			}
			m_aT[nIndex] = t;
		}

		inline T& operator[] (int nIndex)
		{
			return m_aT[nIndex];
		}

		inline T& Get(int nIndex)
		{
			return m_aT[nIndex];
		}

	private:
		T *m_aT;
		int m_nSize;
	};
}


