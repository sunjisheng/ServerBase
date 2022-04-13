#include "Thread.h"
#include "CommonDefine.h"

namespace Minicat
{
	CThread::CThread(void):m_bStop(false)
	{
	}

	CThread::~CThread(void)
	{
	}

	void CThread::Start()
	{
		m_Thread = std::thread(std::mem_fn(&CThread::Run), this);
		m_Thread.detach();
	}

	void CThread::Stop()
	{
		m_bStop = true;
	}

	void CThread::Join()
	{
		if (m_Thread.joinable())
		{
			m_Thread.join();
		}
	}

	string CThread::GetID()
	{
		stringstream ss;
		ss << m_Thread.get_id();
		return ss.str();
	}
}