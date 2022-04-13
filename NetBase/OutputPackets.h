/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  发送未确认的包列表
**********************************************************************************/
#pragma once
#include "BufferPool.h"
namespace Minicat
{
	enum
	{
		Max_OutputPackets = 50,
	};
#pragma pack(1)
	struct OutPacketBuf : public PacketBuf
	{
		OutPacketBuf() :m_tmSend(0), m_nRetry(0)
		{

		}

		void Reset()
		{
			PacketBuf::Reset();
			m_tmSend = 0;
			m_nRetry = 0;
		}
		uint64 m_tmSend;
		ushort m_nRetry;
	};
#pragma pack()
	class COutputPackets
	{
	public:
		COutputPackets();
		~COutputPackets();
		int GetCount();
		bool Push(OutPacketBuf *pBuf);
		bool Pick(OutPacketBuf *&pBuf);
		bool Pop();
		void FreeAll();
	public:
		OutPacketBuf m_arrBuf[Max_OutputPackets];
		int m_nHead;
		int m_nCount;
	};
}