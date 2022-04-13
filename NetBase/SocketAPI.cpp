#include "CommonBase.h"
#include "SocketAPI.h"

namespace Minicat
{
	int Connect(SocketID fd, const char *szIP, int nPort)
	{
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(szIP);
		addr.sin_port = htons(nPort);
		int result = connect(fd, (sockaddr*)&addr, sizeof(addr));
		if (result == SOCKET_ERROR)
		{
#if defined(_WINDOWS)
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				return Tcp_Connect_Pending;
			}
#else
			if (errno == EINPROGRESS)
			{
				return Tcp_Connect_Pending;
			}
#endif
			return Tcp_Connect_Error;
		}
		return Tcp_Connect_Success;
	}

	bool Bind(SocketID fd, int nPort)
	{
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(nPort);
		if (::bind(fd, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
		{
			return false;
		}
		return true;
	}

	bool Listen(SocketID fd)
	{
		if (listen(fd, 512) == SOCKET_ERROR)
		{
			return false;
		}
		return true;
	}

	int Send(SocketID fd, const char *szBuffer, int nLen)
	{
		int result = send(fd, szBuffer, nLen, 0);
		if (result == SOCKET_ERROR)
		{
#if defined(_WINDOWS)
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				return 0;
			}
#elif defined(_LINUX)
			if (errno == EAGAIN || errno == EINTR)
			{
				return 0;
			}
#endif
			return SOCKET_ERROR;
		}
		return result;
	}

	//udp
	int SendTo(SocketID fd, const char *szBuffer, int nLen, sockaddr *to, int addrlen)
	{
		return ::sendto(fd, szBuffer, nLen, 0, (const struct sockaddr *)to, addrlen);
	}

	SocketID Accept(SocketID fd, char *szIP, int nIPLen, int &nPort)
	{
		sockaddr_in addr;
		socklen_t addrlen = sizeof(sockaddr_in);
		SocketID newfd = (SocketID)::accept(fd, (sockaddr*)&addr, &addrlen);
		if (newfd != INVALID_SOCKET)
		{
			strncpy(szIP, inet_ntoa(addr.sin_addr), nIPLen - 1);
			nPort = ntohs(addr.sin_port);
		}
		return newfd;
	}

	int Receive(SocketID fd, char *szBuffer, int nLen)
	{
		int result = recv(fd, szBuffer, nLen, 0);
		if (result == SOCKET_ERROR)
		{
#if defined(_WINDOWS)
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				return 0;
			}
#elif defined(_LINUX)
			if (errno == EAGAIN || errno == EINTR)
			{
				return 0;
			}
#endif
			return SOCKET_ERROR;
		}
		else if (result == 0)
		{
			return SOCKET_ERROR;//连接关闭
		}
		return result;
	}

	//udp
	int RecvFrom(SocketID fd, char *szBuffer, int nLen, sockaddr *from, socklen_t* addrlen)
	{
		return ::recvfrom(fd, szBuffer, nLen, 0, from, addrlen);
	}

	bool SetNonBlocking(SocketID fd, bool on)
	{
		unsigned long ul = (on == true) ? 1 : 0;
#if defined(_WINDOWS)
		if (ioctlsocket(fd, FIONBIO, (unsigned long*)&ul) == SOCKET_ERROR)
		{
			closesocket(fd);
			return FALSE;
		}
#elif defined(_LINUX)
		if (ioctl(fd, FIONBIO, (unsigned long*)&ul) == SOCKET_ERROR)
		{
			close(fd);
			return false;
		}
#endif
		return true;
	}

	bool SetNoDelay(SocketID fd)
	{
#if defined(_WINDOWS)
		BOOL enable = 1;
#elif defined(_LINUX)
		int enable = 1;
#endif
		return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&enable, sizeof(enable)) == 0;
	}

	bool SetLinger(SocketID fd, bool on, int time)
	{
		struct linger li;
		li.l_linger = time;
		li.l_onoff = on;
		return	setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char *)&li, sizeof(li));
	}

	bool SetReuseAddr(SocketID fd)
	{
#if defined(_WINDOWS)
		BOOL bReuseAddr = TRUE;
		return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&bReuseAddr, sizeof(BOOL)) == 0;
#elif defined(_LINUX)
		int reuse_addr = 1;
		return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&reuse_addr, sizeof(reuse_addr)) == 0;
#endif
	}

	void Close(SocketID fd)
	{
#if defined(_WINDOWS)
		closesocket(fd);
#elif defined(_LINUX)
		close(fd);
#endif
	}

	bool GetSocketOpt(SocketID fd, int level, int optname, void* optval, socklen_t* optlen)
	{
#if defined(_WINDOWS)
		if (getsockopt(fd, level, optname, (char*)optval, (int*)optlen) == SOCKET_ERROR)
		{
			return false;
		}
#elif defined(_LINUX)
		if (getsockopt(fd, level, optname, optval, optlen) == SOCKET_ERROR)
		{
			return false;
		}
#endif	
		return true;
	}

	bool SetSocketOpt(SocketID fd, int level, int optname, const void *optval, socklen_t optlen)
	{
#if defined(_WINDOWS)
		if (setsockopt(fd, level, optname, (char*)optval, optlen) == SOCKET_ERROR)
		{
			return false;
		}
#elif defined(_LINUX)
		if (setsockopt(fd, level, optname, optval, optlen) == SOCKET_ERROR)
		{
			return false;
		}
#endif	
		return true;
	}
		
	bool IsSocketError(SocketID fd)
	{
		int error;
		socklen_t len = sizeof(error);

		if (GetSocketOpt(fd, SOL_SOCKET, SO_ERROR, &error, &len))
		{
			if (error == 0)
			{
				return false;
			}
		}
		return true;
	}

	int SocketPair(int fds[2])
	{
#if defined(_WINDOWS)
		//监听
		SocketID listenfd = (SocketID)::socket(AF_INET, SOCK_STREAM, 0);
		struct sockaddr_in listen_addr;
		memset(&listen_addr, 0, sizeof(listen_addr));
		listen_addr.sin_family = AF_INET;
		listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		listen_addr.sin_port = 0;
		::bind(listenfd, (struct sockaddr *) &listen_addr, sizeof(listen_addr));
		::listen(listenfd, 1);
		//连接	
		SocketID connectfd = socket(AF_INET, SOCK_STREAM, 0);
		struct sockaddr_in connect_addr;
		int size = sizeof(connect_addr);
		::getsockname(listenfd, (struct sockaddr *) &connect_addr, &size);
		::connect(connectfd, (struct sockaddr *)&connect_addr, sizeof(connect_addr));
		//接受	
		size = sizeof(listen_addr);
		SocketID acceptfd = ::accept(listenfd, (struct sockaddr *) &listen_addr, &size);
	    //关闭监听
		::closesocket(listenfd);
		fds[0] = connectfd;
		fds[1] = acceptfd;
		return 0;
#elif defined(_LINUX)
		return socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
#endif
	}
	
}