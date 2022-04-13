/*********************************************************************************
  *����:  SunJiSheng
  *����:  2020/04/15
  *����:  ���ڱ�׼��ʵ�ֵ�Thread��
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
		virtual void Run() {};						//�߳����к���
		void Start();								//����
		void Stop();								//�ⲿֹͣ
		inline bool IsStop();						//�Ƿ�ֹͣ
		void Join();								//�ȴ��߳̽���
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