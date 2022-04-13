/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  可靠Udp协议
**********************************************************************************/
#pragma once
#include "CommonBase.h"

namespace Minicat
{
#pragma pack(1)  
	enum UdpCmdID
	{
		Udp_Connect = 1 << 0,     //连接
		Udp_Close = 1 << 1,		  //关闭
		Udp_Data = 1 << 2,        //数据
		Udp_Ack = 1 << 3,         //应答
	};

	/*可靠UDP协议头---|cmd|len|seq|ack|srcsockid|dstsockid| ---*/
	struct UdpHeader
	{
		UdpHeader():nCmd(0),nLen(0), nSequence(0), nAck(0),nSrcSockID(INVALID_SOCKET),nDstSockID(INVALID_SOCKET)
		{

		}
		ushort nLen;              //包长度，用于在一个MTU内尽可能组包发送
		ushort nCmd;              //命令
		uint nSequence;			  //序号 
		uint nAck;                //确认序号字段有效,为期望序号
		SocketID nSrcSockID;      //发送方ID  
		SocketID nDstSockID;      //接收方ID
	};
#pragma pack()
}