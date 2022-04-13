#include "CommonBase.h"
#include "TcpManager.h"
#include "SelectPollImpl.h"
#include "EpollImpl.h"
#include "SocketAPI.h"
#include "NetBase.h"
#include "SocketFactory.h"
#include "Logger.h"
namespace Minicat
{
	void OnSocketRead(SocketID nSocketID)
	{
		CTcpManager::Instance()->OnSocketRead(nSocketID);
	}

	void OnSocketWrite(SocketID nSocketID)
	{
		CTcpManager::Instance()->OnSocketWrite(nSocketID);
	}

	CTcpManager::CTcpManager() 
		: m_pSocketPoll(nullptr)
		, m_fnOnConnect(nullptr)
		, m_fnOnMsg(nullptr)
		, m_fnOnClose(nullptr)
	{
#if defined(_WINDOWS)
		WORD version = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(version, &data);
#endif
	}

	CTcpManager::~CTcpManager()
	{
		SAFE_DELETE(m_pSocketPoll);
		SAFE_DELETE(m_pSocketPair);

		CHashMap<SocketID, CTcpSocket*>::HashNode *pNode = m_mapSockets.Begin();
		while (pNode != NULL)
		{
			SAFE_DELETE(pNode->m_Value);
			pNode = pNode->Next();
		}

#if defined(_WINDOWS)
		WSACleanup();
#endif
	}

	void CTcpManager::RegisterOnConnect(ConnectHandler fnOnConnect)
	{
		m_fnOnConnect = fnOnConnect;
	}

	void CTcpManager::RegisterOnCreateMsg(CreateMsgHandler fnOnCreateMsg)
	{
		m_fnOnCreateMsg = fnOnCreateMsg;
	}

	void CTcpManager::RegisterOnMsg(MsgHandler fnOnMsg)
	{
		m_fnOnMsg = fnOnMsg;
	}

	void CTcpManager::RegisterOnClose(CloseHandler fnOnClose)
	{
		m_fnOnClose = fnOnClose;
	}

	bool CTcpManager::Init()
	{
#if defined(_WINDOWS)
		m_pSocketPoll = new SelectPollImpl();
		if (!m_pSocketPoll)
		{
			return false;
		}
#else
		m_pSocketPoll = new EpollImpl();
		if (!m_pSocketPoll)
		{
			return false;
		}
#endif
		m_pSocketPair = new CSocketPair();
		if (!m_pSocketPair)
		{
			return false;
		}
		m_pSocketPoll->SetReadHandler(Minicat::OnSocketRead);
		m_pSocketPoll->SetWriteHandler(Minicat::OnSocketWrite);
		m_queueAct.Init(Max_QueueAct_Len);
		m_queueEvent.Init(Max_QueueEvent_Len);
		m_pSocketPoll->AddSocket(m_pSocketPair->GetSocket(SocketPair_Second), Socket_Read);
		CThread::Start();
		return true;
	}

	bool CTcpManager::Listen(int nPort)
	{
		ListenAct act;
		act.nActID = NetAct_Listen;
		act.nPort = nPort;
		m_queueAct.Write((char*)&act, sizeof(act));
		NotifyAct();
		return true;
	}

	bool CTcpManager::Connect(const char *szIP, int nPort)
	{
		ConnectAct act;
		act.nActID = NetAct_Connect;
		strncpy(act.szIP, szIP, sizeof(act.szIP) - 1);
		act.nPort = nPort;
		m_queueAct.Write((char*)&act, sizeof(act));
		NotifyAct();
		return true;
	}

	bool CTcpManager::SendMsg(SocketID nSocketID, CMsg *pMsg)
	{
		CTcpSocket *pSocket = nullptr;
		if (!m_mapSockets.Get(nSocketID, pSocket))
		{
			//WriteLog(Log_Level_Error, "CTcpManager::SendMsg SocketID %d Not Found msgid=%d", nSocketID, pMsg->MsgID());
			return false;
		}
		MsgHeader header(pMsg->MsgID(), pMsg->Length());
		if (!pSocket->m_OutStream.Write((char*)&header, sizeof(header)) ||
			!pMsg->Write(pSocket->m_OutStream))
		{
			WriteLog(Log_Level_Error, "CTcpManager::SendMsg m_OutStream Write return FALSE, fd=%d streamsize=%d msglen=%d" , nSocketID, pSocket->m_OutStream.GetSize(), pMsg->Length());
			return false;
		}
		SendAct act;
		act.nActID = NetAct_Send;
		act.nSocketID = nSocketID;
		m_queueAct.Write((char*)&act, sizeof(act));
		NotifyAct();
		return true;
	}

	bool CTcpManager::Close(SocketID nSocketID)
	{
		CloseAct act;
		act.nActID = NetAct_Close;
		act.nSocketID = nSocketID;
		m_queueAct.Write((char*)&act, sizeof(act));
		NotifyAct();
		return true;
	}

	void CTcpManager::Run()
	{
		while (!m_bStop)
		{
			m_pSocketPoll->Poll();
		}
	}

	void CTcpManager::ProcessAct()
	{
		while (m_queueAct.GetSize() > 0)
		{
			int nActID = 0;
			if (!m_queueAct.Peek((char*)&nActID, sizeof(nActID)))
			{
				return;
			}
			switch (nActID)
			{
			case NetAct_Listen:
			{
				ListenAct act;
				if (!m_queueAct.Read((char*)&act, sizeof(act)))
				{
					return;
				}
				InnerListen(act.nPort);
			}
			break;
			case NetAct_Connect:
			{
				ConnectAct act;
				if (!m_queueAct.Read((char*)&act, sizeof(act)))
				{
					return;
				}
				InnerConnect(act.szIP, act.nPort);
			}
			break;
			case NetAct_Send:
			{
				SendAct act;
				if (!m_queueAct.Read((char*)&act, sizeof(act)))
				{
					return;
				}
				m_pSocketPoll->ModSocket(act.nSocketID, Socket_All);
			}
			break;
			case NetAct_Close:
			{
				CloseAct act;
				if (!m_queueAct.Read((char*)&act, sizeof(act)))
				{
					return;
				}
				InnerClose(act.nSocketID);
			}
			break;
			}
		}
	}

	bool CTcpManager::InnerListen(int nPort)
	{
		bool bRet = m_Listener.Listen(nPort);
		if (!bRet)
		{
			WriteLog(Log_Level_Error, "Listen Fail, Port=%d", nPort);
			return false;
		}
		m_pSocketPoll->AddSocket(m_Listener.m_nSocketID, Socket_Read);
		return true;
	}

	bool CTcpManager::InnerConnect(const char *szIP, int nPort)
	{
		CTcpSocket *pSocket = CSocketFactory<CTcpSocket>::Instance()->Alloc();
		if (!pSocket)
		{
			return false;
		}
		if (pSocket->Connect(szIP, nPort) == Tcp_Connect_Error)
		{
			CSocketFactory<CTcpSocket>::Instance()->Free(pSocket);
			PushConnectEvent(NetEvent_Connect, INVALID_SOCKET, szIP, nPort);
			return false;
		}
		m_mapSockets.Set(pSocket->m_nSocketID, pSocket);
		m_pSocketPoll->AddSocket(pSocket->m_nSocketID, Socket_Write);
		return true;
	}

	bool CTcpManager::InnerClose(SocketID nSocketID)
	{
		CTcpSocket *pSocket = nullptr;
		if (!m_mapSockets.Get(nSocketID, pSocket))
		{
			return false;
		}
		InnerClose(pSocket);
		return true;
	}

	bool CTcpManager::InnerClose(CTcpSocket *pSocket)
	{
  		m_mapSockets.Erase(pSocket->m_nSocketID);
		m_pSocketPoll->DelSocket(pSocket->m_nSocketID, Socket_All);
		PushConnectEvent(NetEvent_Close, pSocket->m_nSocketID, pSocket->m_szIP, pSocket->m_nPort);
		pSocket->Close();
		CSocketFactory<CTcpSocket>::Instance()->Free(pSocket);
		return true;
	}

	void CTcpManager::OnSocketRead(SocketID nSocketID)
	{
		if (nSocketID == m_pSocketPair->GetSocket(SocketPair_Second))
		{
			m_pSocketPair->Consume();
			ProcessAct();  //处理上层通知
		}
		else if (nSocketID == m_Listener.m_nSocketID)  
		{
			//新连接到达
			do
			{
				CTcpSocket *pSocket = m_Listener.Accept();
				if (!pSocket)
				{
					break;
				}
				m_mapSockets.Set(pSocket->m_nSocketID, pSocket);
				m_pSocketPoll->AddSocket(pSocket->m_nSocketID, Socket_Read);
				PushConnectEvent(NetEvent_Connect, pSocket->m_nSocketID, pSocket->m_szIP, pSocket->m_nPort);
			} while (true);
		}
		else
		{
			//数据到达
			CTcpSocket *pSocket = nullptr;
			if (!m_mapSockets.Get(nSocketID, pSocket))
			{
				return;
			}
			if (!pSocket->Receive())
			{
				InnerClose(pSocket);
				return;
			}

			while (true)
			{
				//处理消息
				MsgHeader header;
				if (!pSocket->m_InStream.Peek((char*)&header, sizeof(header)))
				{
					break;
				}
				//消息太大
				if (header.nLen >= Max_Msg_Len)
				{
					WriteLog(Log_Level_Error, "Msg too large %d", header.nLen);
					InnerClose(pSocket);
					return;
				}
				//消息未收全
				if (pSocket->m_InStream.GetSize() < sizeof(header) + header.nLen)
				{
					WriteLog(Log_Level_Error, "Msg Not Complete %d(%d,%d) < %d ", pSocket->m_InStream.GetSize(), pSocket->m_InStream.GetHead(), pSocket->m_InStream.GetTail(), sizeof(header) + header.nLen);
					return;
				}
				CMsg *pMsg = m_fnOnCreateMsg(header.nMsgID);
				if (!pMsg)
				{
					WriteLog(Log_Level_Error, "CreateMsg return NULL, MsgID=%d", header.nMsgID);
					InnerClose(pSocket);
					return;
				}
				pSocket->m_InStream.Skip(sizeof(header));
				if (!pMsg->Read(pSocket->m_InStream))
				{
					WriteLog(Log_Level_Error, "Msg Read return FALSE, MsgID=%d", header.nMsgID);
					InnerClose(pSocket);
					return;
				}
				PushRecvEvent(NetEvent_Msg, pSocket->m_nSocketID, pMsg);
			}
		}
	}

	void CTcpManager::OnSocketWrite(SocketID nSocketID)
	{
		CTcpSocket *pSocket = nullptr;
		if (!m_mapSockets.Get(nSocketID, pSocket))
		{
			return;
		}
		if (pSocket->m_nStatus == Tcp_Connect_Pending)
		{
			if (IsSocketError(nSocketID))
			{
				pSocket->m_nStatus = Tcp_Connect_Error;
				PushConnectEvent(NetEvent_Connect, INVALID_SOCKET, pSocket->m_szIP, pSocket->m_nPort);
			}
			else
			{
				pSocket->m_nStatus = Tcp_Connect_Success;
				PushConnectEvent(NetEvent_Connect, nSocketID, pSocket->m_szIP, pSocket->m_nPort);
				m_pSocketPoll->ModSocket(pSocket->m_nSocketID, Socket_Read);
				if (pSocket->GetOutStreamSize() > 0)
				{
					if (!pSocket->Send())
					{
						InnerClose(pSocket);
					}
				}
			}
		}
		else
		{
			if (pSocket->Send())
			{
				m_pSocketPoll->ModSocket(nSocketID, Socket_Read);
			}
			else
			{
				InnerClose(pSocket);
			}
		}
	}

	bool CTcpManager::PushEvent(int nEventID, SocketID nSocketID)
	{
		NetEvent ev;
		ev.nEventID = nEventID;
		ev.nSocketID = nSocketID;
		return m_queueEvent.Write((char*)&ev, sizeof(ev));
	}

	bool CTcpManager::PushConnectEvent(int nEventID, SocketID nSocketID, const char *szIP, int nPort)
	{
		ConnectEvent ev;
		ev.nEventID = nEventID;
		ev.nSocketID = nSocketID;
		strncpy(ev.szIP, szIP, sizeof(ev.szIP) - 1);
		ev.nPort = nPort;
		return m_queueEvent.Write((char*)&ev, sizeof(ev));
	}

	bool CTcpManager::PushRecvEvent(int nEventID, SocketID nSocketID, CMsg *pMsg)
	{
		MsgEvent ev;
		ev.nEventID = nEventID;
		ev.nSocketID = nSocketID;
		ev.pMsg = pMsg;
		return m_queueEvent.Write((char*)&ev, sizeof(ev));
	}

	void CTcpManager::NotifyAct()
	{
		static char szBuf = 'x';
		Minicat::Send(m_pSocketPair->GetSocket(SocketPair_First), &szBuf, 1);
	}

	//主线程中调用
	void CTcpManager::ProcessEvent()
	{
		while (m_queueEvent.GetSize() > 0)
		{
			int nEventID = 0;
			if (!m_queueEvent.Peek((char*)&nEventID, sizeof(nEventID)))
			{
				return;
			}
			if (nEventID == NetEvent_Connect)
			{
				ConnectEvent ev;
				if (!m_queueEvent.Read((char*)&ev, sizeof(ev)))
				{
					return;
				}

				if (m_fnOnConnect)
				{
					(*m_fnOnConnect)(ev.nSocketID, ev.szIP, ev.nPort);
				}
			}
			else if (nEventID == NetEvent_Msg)
			{
				MsgEvent ev;
				if (!m_queueEvent.Read((char*)&ev, sizeof(ev)))
				{
					return;
				}

				if (m_fnOnMsg)
				{
					(*m_fnOnMsg)(ev.nSocketID, ev.pMsg);
				}
			}
			else if (nEventID == NetEvent_Close)
			{
				ConnectEvent ev;
				if (!m_queueEvent.Read((char*)&ev, sizeof(ev)))
				{
					return;
				} 

				if (m_fnOnClose)
				{
					(*m_fnOnClose)(ev.nSocketID, ev.szIP, ev.nPort);
				}
			}
		}
	}
}