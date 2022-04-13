#include "CommonBase.h"
#include "HttpManager.h"
#include "SelectPollImpl.h"
#include "EpollImpl.h"
#include "SocketAPI.h"
#include "NetBase.h"
#include "SocketFactory.h"
#include "Logger.h"
#include "GlobalFunction.h"
namespace Minicat
{
	void OnHttpSocketRead(SocketID nSocketID)
	{
		CHttpManager::Instance()->OnSocketRead(nSocketID);
	}

	void OnHttpSocketWrite(SocketID nSocketID)
	{
		CHttpManager::Instance()->OnSocketWrite(nSocketID);
	}

	CHttpManager::CHttpManager()
		: m_pSocketPoll(nullptr)
		, m_fnOnHttpRequest(nullptr)
		, m_fnOnHttpResponse(nullptr)
	{
#if defined(_WINDOWS)
		WORD version = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(version, &data);
#endif
	}

	CHttpManager::~CHttpManager()
	{
		SAFE_DELETE(m_pSocketPoll);
		SAFE_DELETE(m_pSocketPair);

		CHashMap<SocketID, CHttpSocket*>::HashNode *pNode = m_mapSockets.Begin();
		while (pNode != NULL)
		{
			SAFE_DELETE(pNode->m_Value);
			pNode = pNode->Next();
		}

#if defined(_WINDOWS)
		WSACleanup();
#endif
	}

	void CHttpManager::RegisterRequestHandler(HttpRequestHandler fnOnHttpRequest)
	{
		m_fnOnHttpRequest = fnOnHttpRequest;
	}

	void CHttpManager::RegisterResponseHandler(HttpResponseHandler fnOnHttpResponse)
	{
		m_fnOnHttpResponse = fnOnHttpResponse;
	}

	bool CHttpManager::Init()
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
		m_pSocketPoll->SetReadHandler(Minicat::OnHttpSocketRead);
		m_pSocketPoll->SetWriteHandler(Minicat::OnHttpSocketWrite);
		m_queueAct.Init(Max_QueueAct_Len);
		m_queueEvent.Init(Max_QueueEvent_Len);
		m_pSocketPoll->AddSocket(m_pSocketPair->GetSocket(SocketPair_Second), Socket_Read);
		CThread::Start();
		return true;
	}

	bool CHttpManager::Listen(int nPort)
	{
		ListenAct act;
		act.nActID = HttpAct_Listen;
		act.nPort = nPort;
		m_queueAct.Write((char*)&act, sizeof(act));
		NotifyAct();
		return true;
	}

	bool CHttpManager::PostRequest(char *szUrl, HttpRequest *pRequest)
	{
		HttpReqAct act;
		act.nActID = HttpAct_Request;
		strncpy(act.szUrl, szUrl, Max_URL_Len - 1);
		act.pRequest = pRequest;
		m_queueAct.Write((char*)&act, sizeof(act));
		NotifyAct();
		return true;
	}

	bool CHttpManager::AckResponse(SocketID nSocketID, HttpResponse *pResponse)
	{
		CHttpSocket *pSocket = nullptr;
		if (!m_mapSockets.Get(nSocketID, pSocket))
		{
			return false;
		}

		stringstream ss;
		ss << "HTTP/1.1 200 OK\r\n";
		ss << "Content-Type: text/plain\r\n";
		ss << "Content-Length: " << pResponse->nContentLen << "\r\n";
		for (int i = 0; i < pResponse->nHeaderCount; i++)
		{
			ss << pResponse->arrHeaders[i].szName << ": " << pResponse->arrHeaders[i].szValue << "\r\n";
		}
		ss << "\r\n";
		ss << pResponse->szContent;
		pSocket->m_OutStream.Write(ss.str().c_str(), ss.str().length());
		SAFE_DELETE(pResponse);

		HttpAckAct act;
		act.nActID = HttpAct_Response;
		act.nSocketID = nSocketID;
		m_queueAct.Write((char*)&act, sizeof(act));
		NotifyAct();
		return true;
	}

	void CHttpManager::Run()
	{
		while (!m_bStop)
		{
			m_pSocketPoll->Poll();
		}
	}

	void CHttpManager::ProcessAct()
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
			case HttpAct_Listen:
			{
				ListenAct act;
				if (!m_queueAct.Read((char*)&act, sizeof(act)))
				{
					return;
				}
				InnerListen(act.nPort);
			}
			break;
			case HttpAct_Request:
			{
				HttpReqAct act;
				if (!m_queueAct.Read((char*)&act, sizeof(act)))
				{
					return;
				}
				InnerConnect(act);
			}
			break;
			case HttpAct_Response:
			{
				HttpAckAct act;
				if (!m_queueAct.Read((char*)&act, sizeof(act)))
				{
					return;
				}
				m_pSocketPoll->ModSocket(act.nSocketID, Socket_Write);
			}
			break;
			}
		}
	}

	bool CHttpManager::InnerListen(int nPort)
	{
		bool bRet = m_Listener.Listen(nPort);
		if (!bRet)
		{
			WriteLog(Log_Level_Error, "Listen Fail, Port=%d", nPort);
			return false;
		}
		m_pSocketPoll->AddSocket(m_Listener.m_nSocketID, Socket_Read);
		WriteLog(Log_Level_Info, "InnerListen, fd=%d", m_Listener.m_nSocketID);
		return true;
	}

	bool CHttpManager::InnerConnect(HttpReqAct &act)
	{
		CHttpSocket *pSocket = CSocketFactory<CHttpSocket>::Instance()->Alloc();
		if (!pSocket)
		{
			return false;
		}
		pSocket->m_pRequest = act.pRequest;
		pSocket->m_pResponse = new HttpResponse();
		char szHost[Max_Host_Len] = { 0 };
		char szDir[Max_Directory_Len] = { 0 };
		char szDomain[Max_Domain_Len] = { 0 };
		char szIP[Max_IP4_Len] = { 0 };
		int nPort = 80;
		ParseURL(act.szUrl, szHost, sizeof(szHost), szDir, sizeof(szDir));
		strcat(szDir, act.pRequest->GetDir());
		ParseIPPort(szHost, szDomain, nPort);
		ConvertDomain2IP(szDomain, szIP);
		if (pSocket->Connect(szIP, nPort) == Tcp_Connect_Error)
		{
			CSocketFactory<CHttpSocket>::Instance()->Free(pSocket);
			PushHttpResponseEvent(pSocket->m_pRequest, pSocket->m_pResponse);
			return false;
		}

		stringstream ss;
		ss << "POST " << szDir << " HTTP/1.1\r\n";
		ss << "HOST: " << szHost << "\r\n";
		ss << "Content-Type: text/plain\r\n";
		ss << "Content-Length: " << act.pRequest->nContentLen << "\r\n";
		for (int i = 0; i < act.pRequest->nHeaderCount; i++)
		{
			ss << act.pRequest->arrHeaders[i].szName << ": " << act.pRequest->arrHeaders[i].szValue << "\r\n";
		}
		ss << "\r\n";
		ss << act.pRequest->szContent;
		pSocket->m_OutStream.Write(ss.str().c_str(), ss.str().length());


		m_mapSockets.Set(pSocket->m_nSocketID, pSocket);
		m_pSocketPoll->AddSocket(pSocket->m_nSocketID, Socket_Write);
		return true;
	}

 	bool CHttpManager::InnerClose(SocketID nSocketID)
	{
		CHttpSocket *pSocket = nullptr;
		if (!m_mapSockets.Get(nSocketID, pSocket))
		{
			return false;
		}
		InnerClose(pSocket);
		return true;
	}

	bool CHttpManager::InnerClose(CHttpSocket *pSocket)
	{
		m_mapSockets.Erase(pSocket->m_nSocketID);
		m_pSocketPoll->DelSocket(pSocket->m_nSocketID, Socket_All);
		pSocket->Close();
		CSocketFactory<CHttpSocket>::Instance()->Free(pSocket);
		return true;
	}

	void CHttpManager::OnSocketRead(SocketID nSocketID)
	{
		if (nSocketID == m_pSocketPair->GetSocket(SocketPair_Second))
		{
			m_pSocketPair->Consume();
			ProcessAct();  //处理上层通知
		}
		else if (nSocketID == m_Listener.m_nSocketID)
		{
			//新连接到达
			CHttpSocket *pSocket = m_Listener.Accept();
			if (!pSocket)
			{
				return;
			}
			m_mapSockets.Set(pSocket->m_nSocketID, pSocket);
			m_pSocketPoll->AddSocket(pSocket->m_nSocketID, Socket_Read);
		}
		else
		{
			//数据到达
			CHttpSocket *pSocket = nullptr;
			if (!m_mapSockets.Get(nSocketID, pSocket))
			{
				return;
			}
			if (!pSocket->Receive())
			{
				InnerClose(pSocket);
				return;
			}
			if (pSocket->m_pResponse)
			{
				//待定 处理http协议
				int nRet = pSocket->m_HttpParser.ParseResponse(pSocket->m_pResponse, pSocket->m_InStream);
				if (nRet == Parse_OK)
				{
					PushHttpResponseEvent(pSocket->m_pRequest, pSocket->m_pResponse);
					InnerClose(pSocket);
				}
				else if (nRet == Parse_Fail)
				{
					SAFE_DELETE(pSocket->m_pResponse);
					PushHttpResponseEvent(pSocket->m_pRequest, NULL);
					InnerClose(pSocket);
				}
			}
			else
			{
				if (!pSocket->m_pRequest)
				{
					pSocket->m_pRequest = new HttpRequest();
				}
				int nRet = pSocket->m_HttpParser.ParseRequest(pSocket->m_pRequest, pSocket->m_InStream);
				if(nRet == Parse_OK)
				{
					if (!strcmp(pSocket->m_pRequest->GetDir(), "/favicon.ico"))
					{
						InnerClose(pSocket);
					}
					else
					{
						PushHttpRequestEvent(pSocket->m_nSocketID, pSocket->m_pRequest);
					}
				}
				else if (nRet == Parse_Fail)
				{
					InnerClose(pSocket);
				}
			}
		}
	}

	void CHttpManager::OnSocketWrite(SocketID nSocketID)
	{
		CHttpSocket *pSocket = nullptr;
		if (!m_mapSockets.Get(nSocketID, pSocket))
		{
			return;
		}
		if (pSocket->m_nStatus == Tcp_Connect_Pending)
		{
			if (IsSocketError(nSocketID))
			{
				pSocket->m_nStatus = Tcp_Connect_Error;
				PushHttpResponseEvent(pSocket->m_pRequest, pSocket->m_pResponse);
			}
			else
			{
				pSocket->m_nStatus = Tcp_Connect_Success;
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
		else
		{
			pSocket->Send();
			InnerClose(pSocket);
		}
	}

	bool CHttpManager::PushHttpRequestEvent(SocketID nSocketID, HttpRequest *pRequest)
	{
		HttpEvent ev;
		ev.nSocketID = nSocketID;
		ev.pRequest = pRequest;
		ev.pResponse = NULL;
		return m_queueEvent.Write((char*)&ev, sizeof(ev));
	}

	bool CHttpManager::PushHttpResponseEvent(HttpRequest *pRequest, HttpResponse *pResponse)
	{
		HttpEvent ev;
		ev.nSocketID = INVALID_SOCKET;
		ev.pRequest = pRequest;
		ev.pResponse = pResponse;
		return m_queueEvent.Write((char*)&ev, sizeof(ev));
	}

	void CHttpManager::NotifyAct()
	{
		static char szBuf = 'x';
		Minicat::Send(m_pSocketPair->GetSocket(SocketPair_First), &szBuf, 1);
	}

	//主线程中调用
	void CHttpManager::ProcessEvent()
	{
		while (m_queueEvent.GetSize() > 0)
		{
			HttpEvent ev;
			if (!m_queueEvent.Read((char*)&ev, sizeof(ev)))
			{
				return;
			}

			if (ev.pResponse)
			{
				if (m_fnOnHttpResponse)
				{
					(*m_fnOnHttpResponse)(ev.pRequest, ev.pResponse);
					SAFE_DELETE(ev.pRequest)
					SAFE_DELETE(ev.pResponse)
				}
			}
			else
			{
				if (m_fnOnHttpRequest)
				{
					(*m_fnOnHttpRequest)(ev.nSocketID, ev.pRequest);
					SAFE_DELETE(ev.pRequest)
				}
			}
		}
	}
}