/*********************************************************************************
  *����:  SunJiSheng
  *����:  2020/04/15
  *����:  ���յ��Ĳ������İ��б�
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