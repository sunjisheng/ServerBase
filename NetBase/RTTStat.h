/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  RTT计算统计，按linux处理方法来实现
**********************************************************************************/
#pragma once
#include <math.h>
namespace Minicat
{
	enum RTT_Define
	{
		DEFAULT_RTT = 20,     //ms
		DEFAULT_RTTVAR = 10,  //ms
		Min_RTO = 100,          
		Max_RTO = 1000,
	};

	class CRTTStat
	{
	public:
		CRTTStat() : m_nRTT(DEFAULT_RTT), m_nRTTVAR(DEFAULT_RTTVAR)
		{
		}

		inline int Calc(int nRTT)
		{
			m_nRTT = m_nRTT * 0.875 + nRTT * 0.125;					   //平均值
			m_nRTTVAR = m_nRTTVAR * 0.75 + 0.25 * abs(nRTT - m_nRTT);  //平均偏差
			return GetRTO();
		}

		inline int GetRTO()
		{
			int nRTO = m_nRTT + 4 * m_nRTTVAR;  //应该至少覆盖绝大多数可达包不会重传
			if (nRTO < Min_RTO)
			{
				nRTO = Min_RTO;
			}
			else if (nRTO > Max_RTO)
			{
				nRTO = Max_RTO;
			}
			return nRTO;
		}

		int m_nRTT;
		int m_nRTTVAR;
	};
}