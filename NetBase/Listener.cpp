#include "CommonBase.h"
#include "Listener.h"
#include "SocketAPI.h"
#include "SocketFactory.h"
#include "Logger.h"
namespace Minicat
{
	CListener::CListener():m_nSocketID(INVALID_SOCKET)
	{
	}
	
	CListener::~CListener()
	{
		if (m_nSocketID != INVALID_SOCKET)
		{
			Minicat::Close(m_nSocketID);
		}
	}

	bool CListener::Listen(int nPort)
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

	CTcpSocket* CListener::Accept()
	{
		CTcpSocket *pSocket = CSocketFactory<CTcpSocket>::Instance()->Alloc();
		if (!pSocket)
		{
			return nullptr;
		}
		pSocket->m_nSocketID = Minicat::Accept(m_nSocketID, pSocket->m_szIP, sizeof(pSocket->m_szIP), pSocket->m_nPort);
		if (pSocket->m_nSocketID == INVALID_SOCKET) //没有新连接了
		{
			SAFE_DELETE(pSocket);
			return nullptr;
		}
		Minicat::SetNonBlocking(m_nSocketID, true);
		Minicat::SetNoDelay(m_nSocketID);
		Minicat::SetLinger(m_nSocketID, true, 1);
		pSocket->m_nStatus = Tcp_Connect_Success;
		return pSocket;
	}
}