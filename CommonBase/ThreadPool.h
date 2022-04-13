/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  线程池
**********************************************************************************/

#pragma once
#include "CommonBase.h"
#include "CommonType.h"
#include "TSQueue.h"
#include "GlobalFunction.h"
namespace Minicat
{
	template<typename T, int nThreadCount>
	class CThreadPool
	{
	public:
		CThreadPool() :m_bStop(false)
		{
		}
		~CThreadPool()
		{
		}
	public:
		virtual void OnThreadStart(int nThreadIndex) {};
		virtual void OnExecute(int nThreadIndex, T* pTask) {};
	public:
		int GetTaskCount()
		{
			return (int)m_queueTask.GetSize();
		}
		int GetThreadCount()
		{
			return nThreadCount;
		}
		void Init()
		{
			for (int i = 0; i < nThreadCount; i++)
			{
				std::thread t = std::thread(std::mem_fn(&CThreadPool::ThreadFunc), this, i);
				t.detach();
			}
		}
		void Stop()
		{
			m_bStop = true;
		}
		void Push(T *pTask)
		{
			m_queueTask.Push(pTask);
		}
		void ThreadFunc(int nThreadIndex)
		{
			OnThreadStart(nThreadIndex);
			while (!m_bStop)
			{
				T* pTask = nullptr;
				while(m_queueTask.Pop(pTask))
				{
					OnExecute(nThreadIndex, pTask);
				}
				MySleep(10);
			}
		}
	public:
		bool m_bStop;
		std::vector<std::thread> m_veThreads;
		CTSQueue<T*> m_queueTask;
	};
}