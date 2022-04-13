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
		//启动
		void StartTimer(int nInterval);
		//是否定时时间到
		bool CountingTimer();
		//是否暂停状态
		bool IsPaused() { return m_bPause; }
		void Pause(bool bPause);
		//增加间隔时间
		inline void IncInterval(int nIncInterval) { m_nInterval += nIncInterval; }

		//取剩余时间
		int GetRestTime() { return m_nInterval - m_nElapseTime; }
		//取时间间隔
		bool GetInterval() { return m_nInterval; }
	private:
		int m_nTimerID;
		bool m_bPause;
		int  m_nInterval;   //毫秒数
		int m_nElapseTime;  //已经流逝的毫秒数
		uint64 m_nLastTick;    //最后TickCount()
	};
}
