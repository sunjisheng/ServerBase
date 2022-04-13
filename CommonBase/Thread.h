/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  基于标准库实现的Thread类
**********************************************************************************/

#pragma once
#include "CommonBase.h"
#include "CommonType.h"

namespace Minicat
{
	class CThread
	{
	public:
		CThread();
		virtual ~CThread();
	public:
		virtual void Run() {};						//线程运行函数
		void Start();								//启动
		void Stop();								//外部停止
		inline bool IsStop();						//是否停止
		void Join();								//等待线程结束
		string GetID();
	public:
		std::thread m_Thread;
		bool m_bStop;
	};

	inline bool CThread::IsStop()
	{
		return m_bStop;
	}
}