#pragma once
namespace Minicat
{
	class CTimer
	{
	public:

		CTimer();
		~CTimer();

		inline void SetTimerID(int nTimerID){ m_nTimerID = nTimerID; };
		inline int GetTimerID() { return m_nTimerID; };
		//����
		void StartTimer(int nInterval);
		//�Ƿ�ʱʱ�䵽
		bool CountingTimer();
		//�Ƿ���ͣ״̬
		bool IsPaused() { return m_bPause; }
		void Pause(bool bPause);
		//���Ӽ��ʱ��
		inline void IncInterval(int nIncInterval) { m_nInterval += nIncInterval; }

		//ȡʣ��ʱ��
		int GetRestTime() { return m_nInterval - m_nElapseTime; }
		//ȡʱ����
		bool GetInterval() { return m_nInterval; }
	private:
		int m_nTimerID;
		bool m_bPause;
		int  m_nInterval;   //������
		int m_nElapseTime;  //�Ѿ����ŵĺ�����
		uint64 m_nLastTick;    //���TickCount()
	};
}
