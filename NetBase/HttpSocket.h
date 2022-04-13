#pragma once
#include "NetBase.h"
#include "CircleQueue.h"
#include "TcpSocket.h"
#include "HttpDefine.h"
#include "HttpParser.h"
namespace Minicat
{
	class CHttpSocket : public CTcpSocket
	{
	public:
		CHttpSocket();
		~CHttpSocket();
		void Reset();
	public:
		HttpRequest *m_pRequest;
		HttpResponse *m_pResponse;
		CHttpParser m_HttpParser;
	};
}