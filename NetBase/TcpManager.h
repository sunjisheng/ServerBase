/*********************************************************************************
  *����:  SunJiSheng
  *����:  2020/04/15
  *����:  TcpManager�ǵ��߳�Tcp����⣬
**********************************************************************************/
#pragma once
#include "Thread.h"
#include "Singleton.h"
#include "SocketPoll.h"
#include "TcpSocket.h"
#include "CircleQueue.h"
#include "NetBase.h"
#include "HashMap.h"
#include "Listener.h"
#include "List.h"
#include "SocketFactory.h"
#include "SocketPair.h"
#include "Msg.h"
namespace Minicat
{
	class CTcpManager : public CThread, public Singleton<CTcpManager>
	{
	public:
		CTcpManager();
		~CTcpManager();
	public:
		virtual void Run();											//�̺߳���
	public:
		//����ӿ�
		bool Init();										//�����߳�
		bool Listen(int nPort);
		bool Connect(const char *szIP, int nPort);
		bool Close(SocketID nSocketID);
		bool SendMsg(SocketID nSocketID, CMsg *pMsg);

		void RegisterOnConnect(ConnectHandler fnOnConnect);
		void RegisterOnCreateMsg(CreateMsgHandler fnOnCreateMsg);
		void RegisterOnMsg(MsgHandler fnOnMsg);
		void RegisterOnClose(CloseHandler fnOnClose);
		void ProcessEvent();
		//SocketPool�ص��ӿ�
		void OnSocketRead(SocketID nSocketID);
		void OnSocketWrite(SocketID nSocketID);
	protected:
		void NotifyAct();
		void ProcessAct();
		bool InnerListen(int nPort);
		bool InnerConnect(const char *szIP, int nPort);
		bool InnerClose(SocketID nSocketID);
		bool InnerClose(CTcpSocket *pSocket);

		bool PushEvent(int nEventID, SocketID nSocketID);
		bool PushConnectEvent(int nEventID, SocketID nSocketID, const char *szIP, int nPort);
		bool PushRecvEvent(int nEventID, SocketID nSocketID, CMsg *pMsg);
	protected:
		ConnectHandler m_fnOnConnect;
		CreateMsgHandler m_fnOnCreateMsg;
		MsgHandler m_fnOnMsg;
		CloseHandler m_fnOnClose;
		CListener m_Listener;
		SocketPoll *m_pSocketPoll;
		CCircleQueue m_queueAct;
		CCircleQueue m_queueEvent;
		CHashMap<SocketID, CTcpSocket*> m_mapSockets;
		CSocketPair *m_pSocketPair;
	};
}