/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  TcpSocket代表一个连接对象
**********************************************************************************/

#pragma once
#include "CommonType.h"
#include "NetBase.h"
#include "CircleQueue.h"
namespace Minicat
{
	class CTcpSocket
	{
	public:
		CTcpSocket();
		~CTcpSocket();
	public:
		void Reset();
		int Connect(const char *szIP, int nPort);
		bool Send();
		bool Receive();
		void Close();
		int GetOutStreamSize();
	public:
		SocketID m_nSocketID;
		int m_nStatus;
		char m_szIP[Max_IP4_Len];
		int m_nPort;
		CCircleQueue m_InStream;
		CCircleQueue m_OutStream;
	};
}