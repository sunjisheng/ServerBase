#pragma once
#include "CommonBase.h"
#include "SocketPoll.h"
#include "HashMap.h"
namespace Minicat
{
#ifdef _LINUX
	class EpollImpl : public SocketPoll
	{
	public:
		EpollImpl();
		~EpollImpl();
	public:
		virtual void AddSocket(SocketID fd, int nMask);
		virtual void DelSocket(SocketID fd, int nMask);
		virtual void ModSocket(SocketID fd, int nMask);
		virtual void Poll();
	protected:
		int m_epoll;
		epoll_event* m_events;
	};
#endif
}