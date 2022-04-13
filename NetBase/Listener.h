/*********************************************************************************
  *����:  SunJiSheng
  *����:  2020/04/15
  *����:  ��������
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