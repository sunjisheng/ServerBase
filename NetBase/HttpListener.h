/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  监听器类
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