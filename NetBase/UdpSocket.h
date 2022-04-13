/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  UdpSocket对象类
**********************************************************************************/

#pragma once
#include "CommonBase.h"
#include "CircleQueue.h"
#include "OutSlideQueue.h"
#include "TimeScheduler.h"
#include "InputPackets.h"
#include "OutputPackets.h"
#include "TimeEvent.h"
#include "UdpProto.h"
#include "RTTStat.h"
namespace Minicat
{
	enum TimeEventID
	{
		TimerID_Connect = 0,
		TimerID_Send = 1,
		TimerID_DelayAck = 2,
		Max_TimerID = 3,
	};

	enum Const_Define
	{
		Connect_Timeout = 200,  //连接超时
		DelayAck_Timeout = 20,  //延时确认  
		AckConnect_TryCount = 5,//连接确认次数 
		Send_TryCount = 5,      //最大重传次数
	};

	enum Udp_Connect_Status
	{
		Udp_Connect_None = 0,
		Udp_Connect_Syning = 1,
		Udp_Connect_Acking = 2,
		Udp_Connect_Establish = 3,
		Udp_Connect_FinWait = 4,
		Udp_Connect_CloseWait = 5,
		Udp_Connect_Closed = 6,
	};
	
	class CUdpSocket
	{
	public:
		CUdpSocket();
		~CUdpSocket();
	public:
		inline int GetStatus();
		inline void SetStatus(int nStatus);
		inline bool IsPeerFrom(sockaddr_in &addrPeer);
		inline void SetPeerAddr(const char *szIP, int nPort);
		inline bool IsValidSeq2Me(uint nRecvSeq);    //是否是有效的序号
		inline bool IsValidAck2Me(uint nSendAck);	 //是否是有效的确认序号 
		
		int SendPacket(UdpHeader *pPacket);
		void ReqConnect();
		void AckConnect();
		void ReqClose();
		void AckClose();
		void SendAck();
		void SendHeader(ushort nCmd);
		void OnReqConnect(UdpHeader *pPacket, sockaddr_in &addrPeer);
		void OnAckConnect(UdpHeader *pPacket);
		void OnReqClose(UdpHeader *pPacket);
		void OnAckClose(UdpHeader *pPacket);

		bool Send();
		int Recv(char *szBuf, int nLen);
		void OnPacket(UdpHeader *pPacket, sockaddr_in &addrPeer);
		int GetReadSize();
		void SetTimer(int nTimerID, int nFrame);
		void KillTimer(int nTimerID);
		void KillAllTimer();
		void OnTimeout_Connect();
		void OnTimeout_Send();
		void OnTimeout_DelayAck();
		void Reset();
		void Dump(const char *str);
	public:
		int m_nStatus;								//状态
		int m_nRTO;									//超时重传时间
		SocketID m_nSocketID;						//这个ID是程序创建的
		SocketID m_nPeerSocketID;					//对端SocketID
		uint m_nSeq2Peer;							//发送序号，该序号尚未发送
		uint m_nAck2Me;								//确认序号, 小于该序号对方已全部收到
		uint m_nSeq2Me;								//接收序号，小于等于该序号已全部接收
		uint m_nAck2Peer;                           //接收确认，已发送确认的序号
		sockaddr_in m_addrPeer;						//对端地址 
		CRTTStat m_RTTStat;                         //RTT计算器
		CCircleQueue m_InStream;					//输入缓冲区
		COutSlideQueue m_OutStream;					//输出缓冲区(可滑动缓冲区)
		CIuputPackets m_InputPackets;				//到达的不连续包 
		COutputPackets m_OutputPackets;				//发送尚未确认的包
		TimeEvent<CUdpSocket> m_arrTimeEvent[Max_TimerID];		//定时器
	};

	inline void CUdpSocket::SetStatus(int nStatus)
	{
		m_nStatus = nStatus;
	}

	inline int CUdpSocket::GetStatus()
	{
		return m_nStatus;
	}

	inline void CUdpSocket::SetPeerAddr(const char *szIP, int nPort)
	{
		m_addrPeer.sin_family = AF_INET;
		m_addrPeer.sin_addr.s_addr = inet_addr(szIP);
		m_addrPeer.sin_port = htons(nPort);
	}

	inline bool CUdpSocket::IsPeerFrom(sockaddr_in &addrPeer)
	{
		if (m_addrPeer.sin_port != addrPeer.sin_port)
		{
			return false;
		}
		if (memcmp(&m_addrPeer.sin_addr, &addrPeer.sin_addr, sizeof(addrPeer.sin_addr)) != 0)
		{
			return false;
		}
		return true;
	}

	//是否有效的序号
	inline bool CUdpSocket::IsValidSeq2Me(uint nSeq2Me)
	{
		if (nSeq2Me - m_nSeq2Me < Max_InputPackets)  //兼容回环和不回环两种情况
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	//是否有效的确认序号
	inline bool CUdpSocket::IsValidAck2Me(uint nAck2Me)
	{
		if (m_nSeq2Peer == m_nAck2Me)  //已确认过
		{
			return false;
		}
		else 
		{
			if(nAck2Me - m_nAck2Me <= m_nSeq2Peer - m_nAck2Me)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
}