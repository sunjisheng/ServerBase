#include "Peer.h"
#include "TimeScheduler.h"
#include "TcpManager.h"
#include "Logger.h"
namespace Minicat
{
	CPeer::CPeer() : nPeerType(PeerType_Unknow), lPeerID(0), nPort(0), nSocketID(INVALID_SOCKET)
	{
		memset(szIP, 0, sizeof(szIP));
		m_TimeoutEvent.pOwner = this;
		m_TimeoutEvent.fnOnTimer = &CPeer::OnTimeout;
	}

	CPeer::CPeer(int _nPeerType, uint64 _lPeerID, const char* _szIP, int _nPort)
	:nPeerType(_nPeerType), lPeerID(_lPeerID), nPort(_nPort), nSocketID(INVALID_SOCKET)
	{
		strncpy(szIP, _szIP, Max_IP4_Len - 1);
		m_TimeoutEvent.pOwner = this;
		m_TimeoutEvent.fnOnTimer = &CPeer::OnTimeout;
	}

	CPeer::~CPeer()
	{
		KillTimeout();
	}

	bool CPeer::IsIdentified()
	{
		if (nPeerType == PeerType_Unknow)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	void CPeer::SetTimeout(int nFrame)
	{
		KillTimeout();
		CTimeScheduler<CPeer>::Instance()->AddTimer(nFrame, &m_TimeoutEvent);
	}

	void CPeer::KillTimeout()
	{
		if (m_TimeoutEvent.IsScheduling())
		{
			CTimeScheduler<CPeer>::Instance()->DelTimer(&m_TimeoutEvent);
		}
	}

	void CPeer::OnTimeout()
	{
		CTcpManager::Instance()->Close(nSocketID);
		WriteLog(Log_Level_Error, "CPeer::OnTimeout PeerType=%d PeerID=%lld", nPeerType, lPeerID);
	}
}