/*********************************************************************************
  *����:  SunJiSheng
  *����:  2020/04/15
  *����:  ֧�ֶ��̵߳����ü�����
**********************************************************************************/

#pragma once
#include <mutex>
namespace Minicat
{
	class CRefCount
	{
	public:
		CRefCount() : m_nRefCount(0)
		{ 
		};

		~CRefCount(void)
		{
		};

		void AddRef()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_nRefCount++;
		};

		void DelRef()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_nRefCount--;
		};

		void Release()
		{
			m_mutex.lock();
			--m_nRefCount;
			if (m_nRefCount == 0)
			{
				m_mutex.unlock();
				delete this;
			}
			else
			{
				m_mutex.unlock();
			}
		};

		int GetRefCount()
		{ 
			return m_nRefCount; 
		};
	public:
		std::mutex m_mutex;
	private:
		int m_nRefCount;
	};
}