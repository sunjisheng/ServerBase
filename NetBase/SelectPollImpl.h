/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  采用Select API实现的SocketPool接口的实现类
**********************************************************************************/

#pragma once
#include "CommonBase.h"
#include "SocketPoll.h"
#include "HashMap.h"
namespace Minicat
{
#ifdef _WINDOWS
	class SelectPollImpl : public SocketPoll
	{
		enum
		{
			FDSet_Bak = 0,
			FDSet_Cur = 1,
			FDSet_Count = 2,
		};
	public:
		SelectPollImpl();
		~SelectPollImpl();
	public:
		virtual void AddSocket(SocketID fd, int nMask);
		virtual void DelSocket(SocketID fd, int nMask);
		virtual void ModSocket(SocketID fd, int nMask);
		virtual void Poll();
	protected:
		fd_set m_ReadSet[FDSet_Count];
		fd_set m_WriteSet[FDSet_Count];
		fd_set m_ExceptSet[FDSet_Count];
		SocketID m_fdMax;  //Windows下设置为-1即可
		timeval	m_timeout;
	};
#endif
}