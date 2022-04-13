#include "CommonBase.h"
#include "TcpSocket.h"
#include "SocketAPI.h"
#include "Logger.h"
namespace Minicat
{
	CTcpSocket::CTcpSocket(): m_nPort(0), m_nStatus(Tcp_Connect_None), m_nSocketID(INVALID_SOCKET)
	{
		m_InStream.Init(Max_Msg_Len);
		m_OutStream.Init(Max_Msg_Len);
		memset(m_szIP, 0, sizeof(m_szIP));
	}

	CTcpSocket::~CTcpSocket()
	{
		if (m_nSocketID != INVALID_SOCKET)
		{
			Close();
		}
	}

	void CTcpSocket::Reset()
	{
		m_nSocketID = INVALID_SOCKET;
		m_nStatus = Tcp_Connect_None;
		m_nPort = 0;
		memset(m_szIP, 0, sizeof(m_szIP));
		m_InStream.Reset();
		m_OutStream.Reset();
	}

	int CTcpSocket::Connect(const char *szIP, int nPort)
	{
		m_nSocketID = (SocketID)socket(AF_INET, SOCK_STREAM, 0);
		if (m_nSocketID == INVALID_SOCKET)
		{
			return Tcp_Connect_None;
		}
		if (!Minicat::SetNonBlocking(m_nSocketID, true))
		{
			return Tcp_Connect_None;
		}
		if (!Minicat::SetReuseAddr(m_nSocketID))
		{
			return Tcp_Connect_None;
		}
		if (!Minicat::SetNoDelay(m_nSocketID))
		{
			return Tcp_Connect_None;
		}
		strncpy(m_szIP, szIP, sizeof(m_szIP) - 1);
		m_nPort = nPort;
		m_nStatus = Minicat::Connect(m_nSocketID, szIP, nPort);
		return m_nStatus;
	}

	bool CTcpSocket::Send()
	{
		while (m_OutStream.GetSize() > 0)
		{
			int nLen = Minicat::Send(m_nSocketID, m_OutStream.GetReadPtr(), m_OutStream.GetReadLen());
			if (nLen < 0)
			{
				return false;
			}
			else if (nLen > 0)
			{
				m_OutStream.OnReaded(nLen);
			}
		}
		return true;
	}

	bool CTcpSocket::Receive()
	{
		int nMaxLen = 0;
		int nLen = 0;
		int nLoop = 0;
		do
		{
			nLoop++;
			if (nLoop == 2)
			{
				WriteLog(Log_Level_Error, "CTcpSocket::Receive Loop %d,%d", m_InStream.GetHead(), m_InStream.GetTail());
			}
			nMaxLen = m_InStream.GetWriteLen();
			if (nMaxLen <= 0)
			{
				break;
			}
			nLen = Minicat::Receive(m_nSocketID, m_InStream.GetWritePtr(), nMaxLen);
			if (nLen < 0)  //Socket Error
			{
				return false;
			}
			else if (nLen > 0)
			{
				m_InStream.OnWrited(nLen);
			}
			else
			{
				WriteLog(Log_Level_Error, "CTcpSocket::Receive return zero, nMaxLen=%d(%d,%d)", nMaxLen, m_InStream.GetHead(), m_InStream.GetTail());
			}
		} while (nLen == nMaxLen);
		return true;
	}

	void CTcpSocket::Close()
	{
		Minicat::Close(m_nSocketID);
		m_nSocketID = INVALID_SOCKET;
	}

	int CTcpSocket::GetOutStreamSize()
	{
		return m_OutStream.GetSize();
	}
}