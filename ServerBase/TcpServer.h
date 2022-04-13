#pragma once
#include "TcpManager.h"
#include "ServerBaseDefine.h"
#include "Ini.h"
#include "Fps.h"
#include "HashMap.h"
#include "Msg.h"
#include "Peer.h"
#include "TimerList.h"
namespace Minicat
{
	class CTcpServer : public CTimerList
	{
	public:
		CTcpServer();
		virtual ~CTcpServer();
		//基类接口
		virtual void OnStart() {};
		virtual void OnTick() {};
		virtual CMsg* OnCreateMsg(int nMsgID) { return NULL; };
		virtual bool OnIdentify(SocketID fd, CMsg *pMsg) { return false; };
		virtual void OnConnect(int nPeerType, uint64 lPeerID) {};
		virtual bool OnMsg(int nPeerType, uint64 lPeerID, CMsg *pMsg) { return true; };
		virtual void OnClose(int nPeerType, uint64 lPeerID) {};
	public:
		//公共接口
		bool Init();
		void Connect(int nServerType, const char *szServerName);
		bool Connect(int nServerType, uint64 lServerID, const char *szServerName);
		void Send2Server(int nServerType, uint64 lServerID, CMsg *pMsg);
		void Broadcast2Server(int nServerType, CMsg *pMsg);
		void Send2Client(uint64 lPeerID, CMsg *pMsg);
		void CloseServer(int nServerType, uint64 lServerID);
		void CloseClient(uint64 lPeerID);
		void SetPeerInfo(SocketID fd, int nPeerType, uint64 lPeerID);
		void SetMaxServerID(int nPeerType, uint64 lPeerID);
		int GetServerID_Mod(int nPeerType, uint64 lKey);
		int GetServerID_Rand(int nPeerType);
	public:
		inline void SetFps(int nFps) { m_Fps.SetFps(nFps); }
		inline int GetFps() { return m_Fps.m_nFps; }

		inline void SetStop(bool bStop) { m_bStop = bStop; }
		inline bool GetStop() { return m_bStop; }

		inline void SetServerType(int nServerType) { m_nServerType = nServerType; }
		inline int GetServerType() { return m_nServerType; }

		inline void SetServerID(uint64 lServerID) { m_lServerID = lServerID; }
		inline int64 GetServerID() { return m_lServerID; }

		inline void SetServerName(const char *szServerName) { strncpy(m_szServerName, szServerName, Max_ServerName - 1); }
		inline const char* GetServerName() { return m_szServerName; }
	protected:
		bool LoadGlobalIni();
		bool LoadLocalIni();
		void Loop();
		void TickPeerTimeout();
		void StartDBThread();
	public:
		CPeer* m_arrServerPeer[Max_ServerType_Count][Max_ServerID]; //服务器节点
		int m_arrMaxServerID[Max_ServerType_Count];                 //服务类型节点数量
		CHashMap<uint64, CPeer*> m_mapAddr2ServerPeer;              //本服务主动连接的地址 
		CHashMap<uint64, CPeer*> m_mapClientPeer;                   //客户端节点
		CHashMap<SocketID, CPeer*> m_mapSocketID2Peer;				//SocketID->Peer
	protected:
		bool m_bStop;
		int m_nServerType;
		uint64 m_lServerID;
		char m_szServerName[Max_ServerName];
		int m_nPort;
		CIni m_iniGlobal;
		CIni m_iniLocal;
		CFps m_Fps;
	};

	extern CTcpServer *g_pTcpServer;
}