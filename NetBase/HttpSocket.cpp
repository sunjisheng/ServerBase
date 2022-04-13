#include "HttpSocket.h"

namespace Minicat
{
	CHttpSocket::CHttpSocket()
	{
		m_pRequest = NULL;
		m_pResponse = NULL;
		m_HttpParser.Reset();
	}

	CHttpSocket::~CHttpSocket()
	{
	}

	void CHttpSocket::Reset()
	{
		CTcpSocket::Reset();
		m_pRequest = NULL;
		m_pResponse = NULL;
		m_HttpParser.Reset();
	}
}