/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  Reactor模型接口
**********************************************************************************/

#pragma once
#include "CommonBase.h"

namespace Minicat
{
	enum PollType
	{
		Socket_Read = 1 << 0,
		Socket_Write = 1 << 1,
		Socket_All = Socket_Read | Socket_Write,
	};

	typedef	void(*ReadHandler)(SocketID fd);
	typedef	void(*WriteHandler)(SocketID fd);

	class SocketPoll
	{
	public:
		SocketPoll(): m_ReadHandler(nullptr), m_WriteHandler(nullptr)
		{
		};
	public:
		virtual void AddSocket(SocketID fd, int nMask) = 0;
		virtual void DelSocket(SocketID fd, int nMask) = 0;
		virtual void ModSocket(SocketID fd, int nMask) = 0;
		virtual void Poll() = 0;
	public:
		inline void SetReadHandler(ReadHandler handler)
		{
			m_ReadHandler = handler;
		}
		inline void SetWriteHandler(WriteHandler handler)
		{
			m_WriteHandler = handler;
		}
	public:
		ReadHandler m_ReadHandler;
		WriteHandler m_WriteHandler;
	};
}