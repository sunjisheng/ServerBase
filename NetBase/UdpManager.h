/*********************************************************************************
  *����:  SunJiSheng
  *����:  2020/04/15
  *����:  ���߳̿ɿ�Udpʵ��
**********************************************************************************/
#pragma once
#include "Thread.h"
#include "Singleton.h"
#include "SocketPoll.h"
#include "UdpSocket.h"
#include "CircleQueue.h"
#include "NetBase.h"
#include "HashMap.h"
#include "Listener.h"
#include "List.h"
#include "Vector.h"
#include "SocketFactory.h"
#include "UdpProto.h"
#include "BufferPool.h"
#include "SocketPair.h"
namespace Minicat
{
	enum Udp_Const_Def
	{
		Max_UdpSocket_Count = 65535, //���������
		Max_Package_Size = 65536,    //Udp���ݰ���󳤶�
		Max_MTU = 1400,				 //����䵥Ԫ��1400��֤�����Ƭ
	};

	enum ReadSet_Index
	{
		ReadSet_Bak = 0,
		ReadSet_Cur = 1,
		ReadSet_Count = 2,
	};

	enum Poll_Result
	{
		PollResult_Error,
		PollResult_Event,
		PollResult_Timeout
	};
	class CUdpManager : public CThread, public Singleton<CUdpManager>
	{
		friend class CUdpSocket;
	public:
		CUdpManager();
		~CUdpManager();
	public:
		virtual void Run();									//�̺߳���
	public:
		//����ӿ�
		bool Init();										//�����߳�
		bool Listen(int nPort);
		bool Connect(const char *szIP, int nPort);
		bool Close(SocketID nSocketID);
		bool Send(SocketID nSocketID, char *szBuf, int nLen);
		bool RecvByLen(SocketID nSocketID, char *szBuf, int nLen);			//����ָ������
		int Recv(SocketID nSocketID, char *szBuf, int nLen);				//����ȫ��
		void RegisterOnConnect(ConnectHandler fnOnConnect);
		void RegisterOnMsg(MsgHandler fnOnMsg);
		void RegisterOnClose(CloseHandler fnOnClose);
		void NotifyAct();
		void ProcessEvent();
	protected:
		int Poll(int nMS);  //�¼���ѯ
		void ProcessAct();
		bool InnerListen(int nPort);
		bool InnerConnect(const char *szIP, int nPort);
		void InnerSend(SocketID nSocketID);
		bool InnerClose(SocketID nSocketID);

		bool PushEvent(int nEventID, SocketID nSocketID);
		bool PushConnectEvent(int nEventID, SocketID nSocketID, const char *szIP, int nPort);

		bool AddSocket(CUdpSocket *pSocket);
		bool DelSocket(CUdpSocket *pSocket);
		CUdpSocket* GetSocket(SocketID nSocketID);

		void OnPacket(UdpHeader *pPacket, sockaddr_in &addrPeer);
	public:
		CBufferPool m_BufferPool;                       //�����,���ڷ��䲻�����İ�����
	protected:
		SocketID m_nSocketID;							//Udp Socket���
		fd_set	m_ReadSet[ReadSet_Count];				//Select������
		SocketID m_fdMax;
		ConnectHandler m_fnOnConnect;					//���ӻص�
		MsgHandler m_fnOnMsg;                           //���ջص�
		CloseHandler m_fnOnClose;						//�رջص�
		CSocketPair *m_pSocketPair;                     // 
		CCircleQueue m_queueAct;						//�ϲ���Ϊ����	
		CCircleQueue m_queueEvent;						//���ϲ�֪ͨ�¼�����
		CUdpSocket* m_pSockets[Max_UdpSocket_Count];    //Socket����
		bool m_arrBusyPos[Max_UdpSocket_Count];         //�Ѿ���ռ�õ�
		int m_nFreePos;  //����λ��                       //����λ��
		char m_szRecvBuf[Max_Package_Size];				//Udp���ջ�����
	};
}