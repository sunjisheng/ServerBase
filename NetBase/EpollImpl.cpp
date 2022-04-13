#include "CommonBase.h"
#include "EpollImpl.h"
#include "NetBase.h"
#include "Logger.h"
namespace Minicat
{
#ifdef _LINUX
	EpollImpl::EpollImpl()
	{
		m_epoll = epoll_create(Max_Connection);
		if (m_epoll < 0)
		{
			WriteLog(Log_Level_Error, "epoll_create Failure");
			return;
		}

		m_events = new epoll_event[Max_Connection];
		if (m_events == NULL)
		{
			WriteLog(Log_Level_Error, "epoll_event Failure");
			return;
		}
	}

	EpollImpl::~EpollImpl()
	{
		close(m_epoll);
		m_epoll = 0;
		SAFE_DELETE(m_events)
	}

	void EpollImpl::AddSocket(SocketID fd, int nMask)
	{
		epoll_event ev;
		ev.data.fd = fd;
		ev.events = EPOLLRDHUP | EPOLLERR;
		if (nMask & Socket_Read)
		{
			ev.events |= EPOLLIN;
		}
		if (nMask & Socket_Write)
		{
			ev.events |= EPOLLOUT;
		}
		if (epoll_ctl(m_epoll, EPOLL_CTL_ADD, fd, &ev) != 0)
		{
			WriteLog(Log_Level_Error, "AddSocket Failure fd=%d nMask=%d", fd, nMask);
		}
	}

	void EpollImpl::DelSocket(SocketID fd, int nMask)
	{
		epoll_event ev;
		if (epoll_ctl(m_epoll, EPOLL_CTL_DEL, fd, &ev) != 0)
		{
			const char* szError = strerror(errno);
			WriteLog(Log_Level_Error, "DelSocket Failure fd=%d nMask=%d %d %s", fd, nMask, errno, szError);
		}
	}

	void EpollImpl::ModSocket(SocketID fd, int nMask)
	{
		epoll_event ev;
		ev.data.fd = fd;
		ev.events = EPOLLRDHUP | EPOLLERR;
		if (nMask & Socket_Read)
		{
			ev.events |= EPOLLIN;
		}
		if (nMask & Socket_Write)
		{
			ev.events |= EPOLLOUT;
		}
		if (epoll_ctl(m_epoll, EPOLL_CTL_MOD, fd, &ev) != 0)
		{
			WriteLog(Log_Level_Error, "ModSocket Failure fd=%d nMask=%d", fd, nMask);
		}
	}

	void EpollImpl::Poll()
	{
		int nResult = epoll_wait(m_epoll, m_events, Max_Connection, 1000);
		if (nResult < 0) //error
		{
			if (errno == EINTR)
			{
				return;
			}
			else
			{
				const char* szError = strerror(errno);
				WriteLog(Log_Level_Error, "epoll_wait Failure, errno:%d, errstr:%s", errno, szError);
				return;
			}
		}
		else if (nResult == 0)  //timeout
		{
			return;
		}
		else
		{
			for (int i = 0; i < nResult; ++i)
			{
				epoll_event* ev = m_events + i;
				if (ev->events & (EPOLLIN | EPOLLERR | EPOLLRDHUP))
				{
					if (m_ReadHandler)
					{
						(*m_ReadHandler)(ev->data.fd);
					}
				}
				if (ev->events & EPOLLOUT)
				{
					if (m_WriteHandler)
					{
						(*m_WriteHandler)(ev->data.fd);
					}
				}
			}
		}
	}
#endif
}