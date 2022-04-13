#include "CommonBase.h"
#include "UdpManager.h"
#include "SelectPollImpl.h"
#include "SocketAPI.h"
#include "NetBase.h"
#include "UdpProto.h"
#include "GlobalFunction.h"
#include "Random.h"
namespace Minicat
{
	CUdpManager::CUdpManager()
		: m_fnOnConnect(nullptr)
		, m_fnOnMsg(nullptr)
		, m_fnOnClose(nullptr)
		, m_nFreePos(0)
		, m_nSocketID(INVALID_SOCKET)
		, m_fdMax(INVALID_SOCKET)
	{
#if defined(_WINDOWS)
		WORD version = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(version, &data);
#endif
		m_queueAct.Init(65535);
		m_queueEvent.Init(65535);
		memset(m_arrBusyPos, 0, sizeof(m_arrBusyPos));
		memset(m_szRecvBuf, 0, sizeof(m_szRecvBuf));
		//初始换空闲链表
		for (int i = 0; i < Max_UdpSocket_Count; i++)
		{
			*(int*)(&m_pSockets[i]) = i + 1;
		}
		*(int*)(&m_pSockets[Max_UdpSocket_Count - 1]) = -1;
		for (int i = 0; i < ReadSet_Count; i++)
		{
			FD_ZERO(&m_ReadSet[i]);
		}
	}

	CUdpManager::~CUdpManager()
	{
		SAFE_DELETE(m_pSocketPair);
#if defined(_WINDOWS)
		WSACleanup();
#endif
		CBufferPool::Release();
	}

	void CUdpManager::RegisterOnConnect(ConnectHandler fnOnConnect)
	{
		m_fnOnConnect = fnOnConnect;
	}

	void CUdpManager::RegisterOnMsg(MsgHandler fnOnMsg)
	{
		m_fnOnMsg = fnOnMsg;
	}

	void CUdpManager::RegisterOnClose(CloseHandler fnOnClose)
	{
		m_fnOnClose = fnOnClose;
	}

	bool CUdpManager::Init()
	{
		if (m_nSocketID != INVALID_SOCKET)
		{
			return false;
		}

		m_nSocketID = (SocketID)socket(AF_INET, SOCK_DGRAM, 0);
		if (m_nSocketID == INVALID_SOCKET)
		{
			return false;
		}
		if (!Minicat::SetNonBlocking(m_nSocketID, true))
		{
			return false;
		}

		m_pSocketPair = new CSocketPair();
		if (!m_pSocketPair)
		{
			return false;
		}
		FD_SET(m_nSocketID, &m_ReadSet[ReadSet_Bak]);
		SocketID nSocketID = m_pSocketPair->GetSocket(SocketPair_Second);
		FD_SET(nSocketID, &m_ReadSet[ReadSet_Bak]);
		m_fdMax = m_nSocketID > nSocketID ? m_nSocketID : nSocketID;

		CBufferPool::Init();
		CTimeScheduler<CUdpSocket>::Instance()->Init();
		CThread::Start();

		return true;
	}

	bool CUdpManager::Listen(int nPort)
	{
		ListenAct act;
		act.nActID = NetAct_Listen;
		act.nPort = nPort;
		m_queueAct.Write((char*)&act, sizeof(act));
		NotifyAct();
		return true;
	}

	bool CUdpManager::Connect(const char *szIP, int nPort)
	{
		ConnectAct act;
		act.nActID = NetAct_Connect;
		strncpy(act.szIP, szIP, sizeof(act.szIP) - 1);
		act.nPort = nPort;
		m_queueAct.Write((char*)&act, sizeof(act));
		NotifyAct();
		return true;
	}

	bool CUdpManager::Send(SocketID nSocketID, char *szBuf, int nLen)
	{
		CUdpSocket *pSocket = GetSocket(nSocketID);
		if (!pSocket)
		{
			return false;
		}
		if (pSocket->GetStatus() != Udp_Connect_Establish)
		{
			return false;
		}
		if (pSocket->m_OutStream.GetSpace() < sizeof(UdpHeader) + nLen)
		{
			return false;
		}
		UdpHeader header;
		header.nCmd = Udp_Data;
		header.nLen = nLen;
		header.nSrcSockID = nSocketID;
		header.nDstSockID = pSocket->m_nPeerSocketID;
		pSocket->m_OutStream.Write((const char *)&header, sizeof(header));
		pSocket->m_OutStream.Write(szBuf, nLen);

		SendAct act;
		act.nActID = NetAct_Send;
		act.nSocketID = nSocketID;
		return m_queueAct.Write((char*)&act, sizeof(act));
	}

	bool CUdpManager::Close(SocketID nSocketID)
	{
		CloseAct act;
		act.nActID = NetAct_Close;
		act.nSocketID = nSocketID;
		return m_queueAct.Write((char*)&act, sizeof(act));
	}

	bool CUdpManager::RecvByLen(SocketID nSocketID, char *szBuf, int nLen)
	{
		CUdpSocket *pSocket = GetSocket(nSocketID);
		if (!pSocket)
		{
			return false;
		}
		return pSocket->m_InStream.Read(szBuf, nLen);
	}

	int CUdpManager::Recv(SocketID nSocketID, char *szBuf, int nLen)
	{
		CUdpSocket *pSocket = GetSocket(nSocketID);
		if (!pSocket)
		{
			return false;
		}
		return pSocket->Recv(szBuf, nLen);
	}

	void CUdpManager::Run()
	{
		CRandom::SRand();
		uint64 nStart = GetTickCount();
		uint64 nMS = 10;
		while (!m_bStop)
		{
			int nRet = Poll(nMS);
			uint64 nCurr = GetTickCount();
			if (nRet == PollResult_Timeout)
			{
				CTimeScheduler<CUdpSocket>::Instance()->Tick();
				nStart = nStart + 10;
			}
			nMS = nCurr - nStart < 10 ? 10 - (nCurr - nStart) : 0;
		}
	}

	void CUdpManager::ProcessAct()
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
				InnerSend(act.nSocketID);
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

	bool CUdpManager::InnerListen(int nPort)
	{
		if (!Minicat::Bind(m_nSocketID, nPort))
		{
			return false;
		}
		return true;
	}

	bool CUdpManager::InnerConnect(const char *szIP, int nPort)
	{
		CUdpSocket *pSocket = CSocketFactory<CUdpSocket>::Instance()->Alloc();
		if (!pSocket)
		{
			return false;
		}
		pSocket->Reset();
		AddSocket(pSocket);
		pSocket->SetPeerAddr(szIP, nPort);
		pSocket->ReqConnect();
		pSocket->m_nStatus = Udp_Connect_Syning;
		return true;
	}

	void CUdpManager::InnerSend(SocketID nSocketID)
	{
		CUdpSocket *pSocket = GetSocket(nSocketID);
		if (!pSocket)
		{
			return;
		}
		if (pSocket->GetStatus() != Udp_Connect_Establish)
		{
			return;
		}
		pSocket->Send();
		if (pSocket->GetStatus() == Udp_Connect_Closed)
		{
			DelSocket(pSocket);
		}
	}

	bool CUdpManager::InnerClose(SocketID nSocketID)
	{
		CUdpSocket *pSocket = GetSocket(nSocketID);
		if (!pSocket)
		{
			return false;
		}
		pSocket->ReqClose();
		return true;
	}

	bool CUdpManager::PushEvent(int nEventID, SocketID nSocketID)
	{
		NetEvent ev;
		ev.nEventID = nEventID;
		ev.nSocketID = nSocketID;
		return m_queueEvent.Write((char*)&ev, sizeof(ev));
	}

	bool CUdpManager::PushConnectEvent(int nEventID, SocketID nSocketID, const char *szIP, int nPort)
	{
		ConnectEvent ev;
		ev.nEventID = nEventID;
		ev.nSocketID = nSocketID;
		strncpy(ev.szIP, szIP, sizeof(ev.szIP) - 1);
		ev.nPort = nPort;
		return m_queueEvent.Write((char*)&ev, sizeof(ev));
	}

	void CUdpManager::ProcessEvent()
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

	CUdpSocket* CUdpManager::GetSocket(SocketID nSocketID)
	{
		int nPos = nSocketID - 1;
		if (nPos < 0 || nPos >= Max_UdpSocket_Count)
		{
			return nullptr;
		}
		if (!m_arrBusyPos[nPos])
		{
			return nullptr;
		}
		return m_pSockets[nPos];
	}

	bool CUdpManager::AddSocket(CUdpSocket *pSocket)
	{
		if (m_nFreePos < 0)
		{
			return false;
		}
		int nNextPos = *(int*)(&m_pSockets[m_nFreePos]);
		m_pSockets[m_nFreePos] = pSocket;
		m_arrBusyPos[m_nFreePos] = true;
		pSocket->m_nSocketID = m_nFreePos + 1;  //SocketID为数组下标+1
		m_nFreePos = nNextPos;
		return true;
	}

	bool CUdpManager::DelSocket(CUdpSocket *pSocket)
	{
		int nPos = pSocket->m_nSocketID - 1;
		if (nPos < 0 || nPos >= Max_UdpSocket_Count)
		{
			return false;
		}
		if (m_pSockets[nPos] != pSocket)
		{
			return false;
		}
		*(int*)&m_pSockets[nPos] = m_nFreePos;
		m_nFreePos = nPos;
		m_arrBusyPos[nPos] = false;
		PushEvent(NetEvent_Close, pSocket->m_nSocketID);
		pSocket->Reset();
		CSocketFactory<CUdpSocket>::Instance()->Free(pSocket);
		return true;
	}

	int CUdpManager::Poll(int nMS)
	{
		if (m_nSocketID == INVALID_SOCKET)
		{
			return PollResult_Error;
		}
		
		m_ReadSet[ReadSet_Cur] = m_ReadSet[ReadSet_Bak];
		struct timeval tv { 0, nMS * 1000};
		int nRet = ::select(m_fdMax + 1, &m_ReadSet[ReadSet_Cur], NULL, NULL, &tv);
		if (nRet < 0)
		{
			return PollResult_Error;
		}
		else if (nRet == 0)
		{
			return PollResult_Timeout;
		}
		else
		{
			if (FD_ISSET(m_nSocketID, &m_ReadSet[ReadSet_Cur]))
			{
				sockaddr_in addrPeer;
				socklen_t addrlen = sizeof(addrPeer);
				int nLen = Minicat::RecvFrom(m_nSocketID, m_szRecvBuf, Max_Package_Size, (sockaddr*)&addrPeer, &addrlen);
				if (nLen <= 0)
				{
					return PollResult_Error;
				}
				OnPacket((UdpHeader*)m_szRecvBuf, addrPeer);
			}
			if (FD_ISSET(m_pSocketPair->GetSocket(SocketPair_Second), &m_ReadSet[ReadSet_Cur]))
			{
				ProcessAct(); //处理上层通知
			}
			return PollResult_Event;
		}
	}

	void CUdpManager::OnPacket(UdpHeader *pPacket, sockaddr_in &addrPeer)
	{
		CUdpSocket *pSocket = nullptr;
		if (pPacket->nCmd == Udp_Connect)
		{
			pSocket = CSocketFactory<CUdpSocket>::Instance()->Alloc();
			if (!pSocket)
			{
				return;
			}
			pSocket->Reset();
			AddSocket(pSocket);
		}
		else
		{
			pSocket = GetSocket(pPacket->nDstSockID);
			if (!pSocket)
			{
				return;
			}
			if (!pSocket->IsPeerFrom(addrPeer))  //检查地址
			{
				return;
			}
		}

		pSocket->OnPacket(pPacket, addrPeer);
		if (pSocket->GetStatus() == Udp_Connect_Closed)
		{
			DelSocket(pSocket);
		}
	}

	void CUdpManager::NotifyAct()
	{
		if (!m_queueAct.IsEmpty())
		{
			static char szBuf = 'x';
			Minicat::Send(m_pSocketPair->GetSocket(SocketPair_First), &szBuf, 1);
		}
	}
}

