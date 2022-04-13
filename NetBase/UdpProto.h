/*********************************************************************************
  *����:  SunJiSheng
  *����:  2020/04/15
  *����:  �ɿ�UdpЭ��
**********************************************************************************/
#pragma once
#include "CommonBase.h"

namespace Minicat
{
#pragma pack(1)  
	enum UdpCmdID
	{
		Udp_Connect = 1 << 0,     //����
		Udp_Close = 1 << 1,		  //�ر�
		Udp_Data = 1 << 2,        //����
		Udp_Ack = 1 << 3,         //Ӧ��
	};

	/*�ɿ�UDPЭ��ͷ---|cmd|len|seq|ack|srcsockid|dstsockid| ---*/
	struct UdpHeader
	{
		UdpHeader():nCmd(0),nLen(0), nSequence(0), nAck(0),nSrcSockID(INVALID_SOCKET),nDstSockID(INVALID_SOCKET)
		{

		}
		ushort nLen;              //�����ȣ�������һ��MTU�ھ������������
		ushort nCmd;              //����
		uint nSequence;			  //��� 
		uint nAck;                //ȷ������ֶ���Ч,Ϊ�������
		SocketID nSrcSockID;      //���ͷ�ID  
		SocketID nDstSockID;      //���շ�ID
	};
#pragma pack()
}