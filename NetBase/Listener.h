/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  监听器类
**********************************************************************************/

#pragma once
#include "CommonBase.h"
#include "TcpSocket.h"

namespace Minicat
{
	class CListener
	{
	public:
		CListener();
		~CListener();
	public:
		bool Listen(int nPort);
		CTcpSocket* Accept();
	public:
		SocketID m_nSocketID;
	};
}