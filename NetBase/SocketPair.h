/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  SocketPair类，用于实现线程间通知
**********************************************************************************/
#pragma once
#include "CommonBase.h"
#include "SocketAPI.h"
namespace Minicat
{
	enum
	{
		SocketPair_First,
		SocketPair_Second,
	};
	class CSocketPair
	{
	public:
		CSocketPair()
		{
			SocketPair(m_fdPair);
			SetNonBlocking(m_fdPair[SocketPair_First], true);
			SetNonBlocking(m_fdPair[SocketPair_Second], true);
		}
		~CSocketPair()
		{
			if (m_fdPair[SocketPair_First] != INVALID_SOCKET)
			{
				Minicat::Close(m_fdPair[SocketPair_First]);
				m_fdPair[SocketPair_First] = INVALID_SOCKET;
			}
			if (m_fdPair[SocketPair_Second] != INVALID_SOCKET)
			{
				Minicat::Close(m_fdPair[SocketPair_Second]);
				m_fdPair[SocketPair_Second] = INVALID_SOCKET;
			}
		}

		SocketID GetSocket(int nPos)
		{
			if (nPos < 0 || nPos >= 2)
			{
				return INVALID_SOCKET;
			}
			return m_fdPair[nPos];
		}

		void Consume()
		{
			static char szBuf[0124];
			Minicat::Receive(m_fdPair[SocketPair_Second], szBuf, sizeof(szBuf));
		}

	private:
		SocketID m_fdPair[2];
	};
}