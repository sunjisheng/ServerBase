/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  接收到的不连续的包列表
**********************************************************************************/
#pragma once
#include "BufferPool.h"
namespace Minicat
{
	enum 
	{
		Max_InputPackets = 50,
	};
#pragma pack(1)
	struct InPacketBuf : public PacketBuf
	{};
#pragma pack()
	class CIuputPackets
	{
	public:
		CIuputPackets();
		~CIuputPackets();
		bool Push(int nDiff, PacketBuf *pBuf);
		bool Pop(PacketBuf *pBuf);
		void FreeAll();
	public:
		PacketBuf m_arrBuf[Max_InputPackets];
		int m_nHead;
		int m_nDiff;
	};
}