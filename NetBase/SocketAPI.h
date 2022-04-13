/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  socket api
**********************************************************************************/

#pragma once
#include "CommonBase.h"
namespace Minicat
{
	enum Tcp_Connect_Status
	{
		Tcp_Connect_None = 0,
		Tcp_Connect_Pending = 1,
		Tcp_Connect_Success = 2,
		Tcp_Connect_Error = 3,
	};

	int Connect(SocketID fd, const char *szIP, int nPort);
	bool Bind(SocketID fd, int nPort);
	bool Listen(SocketID fd);
	SocketID Accept(SocketID fd, char *szIP, int nIPLen, int &nPort);
	int Send(SocketID fd, const char *szBuffer, int nLen);
	int SendTo(SocketID fd, const char *szBuffer, int nLen, sockaddr *to, int addrlen);
	int Receive(SocketID fd, char *szBuffer, int nLen);
	int RecvFrom(SocketID fd, char *szBuffer, int nLen, sockaddr *from, socklen_t* addrlen);
	bool SetNonBlocking(SocketID fd, bool on);
	bool SetNoDelay(SocketID fd);
	bool SetLinger(SocketID fd, bool on, int time);
	bool SetReuseAddr(SocketID fd);
	void Close(SocketID fd);
	bool GetSocketOpt(SocketID fd, int level, int optname, void* optval, socklen_t* optlen);
	bool SetSocketOpt(SocketID fd, int level, int optname, const void *optval, socklen_t optlen);
	bool IsSocketError(SocketID fd);
	int SocketPair(int fds[2]);
}