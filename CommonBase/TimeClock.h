/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  时钟类
**********************************************************************************/

#pragma once
#include "CommonBase.h"

namespace Minicat
{
	class CTimeClock
	{
	public:
		
		static void Update();  
		static time_t GetTime();  //当前时间
		static uint64 GetTick();

		static inline int GetYear();
		static inline int GetMonth();
		static inline int GetDay();
		static inline int GetHour();
		static inline int GetMinute();
		static inline int GetSecond();
	private:
		static void Convert2TM(const time_t *_tmt, tm *_tm);
	public:
		static time_t m_nTime;
		static tm m_tm;
		static uint64 m_nTick;
	};

	inline int CTimeClock::GetYear()
	{
		return m_tm.tm_year + 1900;
	};

	inline int CTimeClock::GetMonth()
	{
		return m_tm.tm_mon + 1;
	};

	inline int CTimeClock::GetDay()
	{
		return m_tm.tm_mday;
	};

	inline int CTimeClock::GetHour()
	{
		return m_tm.tm_hour;
	};

	inline int CTimeClock::GetMinute()
	{
		return m_tm.tm_min;
	};

	inline int CTimeClock::GetSecond()
	{
		return m_tm.tm_sec;
	};
}