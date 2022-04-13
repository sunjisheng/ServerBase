/*********************************************************************************
  *����:  SunJiSheng
  *����:  2020/04/15
  *����:  UdpSocket������
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
		Connect_Timeout = 200,  //���ӳ�ʱ
		DelayAck_Timeout = 20,  //��ʱȷ��  
		AckConnect_TryCount = 5,//����ȷ�ϴ��� 
		Send_TryCount = 5,      //����ش�����
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
		inline bool IsValidSeq2Me(uint nRecvSeq);    //�Ƿ�����Ч�����
		inline bool IsValidAck2Me(uint nSendAck);	 //�Ƿ�����Ч��ȷ����� 
		
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
		int m_nStatus;								//״̬
		int m_nRTO;									//��ʱ�ش�ʱ��
		SocketID m_nSocketID;						//���ID�ǳ��򴴽���
		SocketID m_nPeerSocketID;					//�Զ�SocketID
		uint m_nSeq2Peer;							//������ţ��������δ����
		uint m_nAck2Me;								//ȷ�����, С�ڸ���ŶԷ���ȫ���յ�
		uint m_nSeq2Me;								//������ţ�С�ڵ��ڸ������ȫ������
		uint m_nAck2Peer;                           //����ȷ�ϣ��ѷ���ȷ�ϵ����
		sockaddr_in m_addrPeer;						//�Զ˵�ַ 
		CRTTStat m_RTTStat;                         //RTT������
		CCircleQueue m_InStream;					//���뻺����
		COutSlideQueue m_OutStream;					//���������(�ɻ���������)
		CIuputPackets m_InputPackets;				//����Ĳ������� 
		COutputPackets m_OutputPackets;				//������δȷ�ϵİ�
		TimeEvent<CUdpSocket> m_arrTimeEvent[Max_TimerID];		//��ʱ��
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

	//�Ƿ���Ч�����
	inline bool CUdpSocket::IsValidSeq2Me(uint nSeq2Me)
	{
		if (nSeq2Me - m_nSeq2Me < Max_InputPackets)  //���ݻػ��Ͳ��ػ��������
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	//�Ƿ���Ч��ȷ�����
	inline bool CUdpSocket::IsValidAck2Me(uint nAck2Me)
	{
		if (m_nSeq2Peer == m_nAck2Me)  //��ȷ�Ϲ�
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