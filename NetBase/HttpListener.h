/*********************************************************************************
  *����:  SunJiSheng
  *����:  2020/04/15
  *����:  ��������
**********************************************************************************/

#pragma once
#include "CommonBase.h"
#include "HttpSocket.h"

namespace Minicat
{
	class CHttpListener
	{
	public:
		CHttpListener();
		~CHttpListener();
	public:
		bool Listen(int nPort);
		CHttpSocket* Accept();
	public:
		SocketID m_nSocketID;
	};
}