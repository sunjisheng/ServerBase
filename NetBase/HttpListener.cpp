#include "CommonBase.h"
#include "HttpListener.h"
#include "SocketAPI.h"
#include "SocketFactory.h"
#include "Logger.h"
namespace Minicat
{
	CHttpListener::CHttpListener() :m_nSocketID(INVALID_SOCKET)
	{
	}

	CHttpListener::~CHttpListener()
	{
		if (m_nSocketID != INVALID_SOCKET)
		{
			Minicat::Close(m_nSocketID);
		}
	}

	bool CHttpListener::Listen(int nPort)
	{
		if (m_nSocketID != INVALID_SOCKET)
		{
			return false;
		}

		m_nSocketID = (SocketID)socket(AF_INET, SOCK_STREAM, 0);
		if (m_nSocketID == INVALID_SOCKET)
		{
			return false;
		}
		if (!Minicat::SetNonBlocking(m_nSocketID, true))
		{
			return false;
		}
		if (!Minicat::Bind(m_nSocketID, nPort))
		{
			return false;
		}
		if (!Minicat::Listen(m_nSocketID))
		{
			return false;
		}
		WriteLog(Log_Level_Info, "Listen Port %d", nPort);
		return true;
	}

	CHttpSocket* CHttpListener::Accept()
	{
		CHttpSocket *pSocket = CSocketFactory<CHttpSocket>::Instance()->Alloc();
		if (!pSocket)
		{
			return nullptr;
		}
		pSocket->m_nSocketID = Minicat::Accept(m_nSocketID, pSocket->m_szIP, sizeof(pSocket->m_szIP), pSocket->m_nPort);
		if (pSocket->m_nSocketID == INVALID_SOCKET)
		{
			SAFE_DELETE(pSocket);
		}
		Minicat::SetNoDelay(m_nSocketID);
		Minicat::SetLinger(m_nSocketID, true, 1);
		pSocket->m_nStatus = Tcp_Connect_Success;
		return pSocket;
	}
}