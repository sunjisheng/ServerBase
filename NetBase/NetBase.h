/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  网络相关定义头文件
**********************************************************************************/
#pragma once
#include "CommonBase.h"
#include "Msg.h"
namespace Minicat
{
	enum
	{
		Max_Connection = 4096,
		Max_IP4_Len = 16,
		Max_Msg_Len = 4096,
		Max_InStream_Len = 16384,
		Max_OutStream_Len = 16384,
		Max_QueueAct_Len = 65536,
		Max_QueueEvent_Len = 65536,
	};

	enum NetAct_ID
	{
		NetAct_Listen = 1,
		NetAct_Connect = 2,
		NetAct_Send = 3,
		NetAct_Close = 4,
	};

	enum NetEvent_ID
	{
		NetEvent_Connect = 1,
		NetEvent_ConnectFail = 2,
		NetEvent_Msg = 3,
		NetEvent_Close = 4,
	};

	struct ListenAct
	{
		int nActID;
		int nPort;
	};

	struct ConnectAct
	{
		int nActID;
		char szIP[Max_IP4_Len];
		int nPort;
	};

	struct SendAct
	{
		int nActID;
		SocketID nSocketID;
	};

	struct RecvAct
	{
		int nActID;
		SocketID nSocketID;
	};

	struct CloseAct
	{
		int nActID;
		SocketID nSocketID;
	};

	struct NetEvent
	{
		int nEventID;
		SocketID nSocketID;
	};

	struct ConnectEvent : public NetEvent
	{
		char szIP[Max_IP4_Len];
		int nPort;
	};

	struct MsgEvent : public NetEvent
	{
		CMsg *pMsg;
	};

	typedef void(*ConnectHandler)(SocketID fd, const char *szIP, int nPort);
	typedef void(*MsgHandler)(SocketID fd, CMsg *pMsg);
	typedef void(*CloseHandler)(SocketID fd, const char *szIP, int nPort);
	typedef CMsg* (*CreateMsgHandler)(int nMsgID);
}
