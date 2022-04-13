/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  用模板实现Singleton
**********************************************************************************/
#pragma once
#include "CommonBase.h"
namespace Minicat
{
	template<typename T>
	class Singleton
	{
	public:
		Singleton() {}
		~Singleton() {}

		static inline T* Instance()
		{
			if (m_pT == nullptr)
			{
				m_pT = (T*)malloc(sizeof(T));
				if (m_pT)
				{
					new(m_pT)T();
				}
			}
			return m_pT;
		}

		static inline void SetInstance(T *pT)
		{
			m_pT = pT;
		}
	private:
		Singleton(const Singleton&);
		const Singleton& operator=(const Singleton&);
	private:
		static T* m_pT;
	};

	template<typename T>
	T* Singleton<T>::m_pT = nullptr;
}