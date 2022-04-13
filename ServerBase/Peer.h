#pragma once
#include "CommonBase.h"
#include "NetBase.h"
#include "ServerBaseDefine.h"
#include "TimeEvent.h"
namespace Minicat
{
	class CPeer
	{
	public:
		CPeer();
		CPeer(int _nPeerType, uint64 _lPeerID, const char* _szIP, int _nPort);
		~CPeer();

		void OnTimeout();
		void SetTimeout(int nFrame);
		void KillTimeout();
		bool IsIdentified();
	public:
		int nPeerType;
		uint64 lPeerID;
		SocketID nSocketID;
		char szIP[Max_IP4_Len];
		int nPort;
		uint nLastRecvTime;
		TimeEvent<CPeer> m_TimeoutEvent; //¶¨Ê±Æ÷
	};
}

