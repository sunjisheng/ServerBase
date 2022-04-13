#include "CommonBase.h"
#include "TcpServer.h"
#include "TcpManager.h"
#include "GlobalFunction.h"
#include "Logger.h"
#include "ServerConnectMsg.h"
#include "HeartbeatMsg.h"
#include "TimeScheduler.h"
#include "Logger.h"
#include "Random.h"
#include "MySqlManager.h"
namespace Minicat
{
	CTcpServer *g_pTcpServer = NULL;
	//网络事件回调处理
	void OnConnect(SocketID fd, const char *szIP, int nPort);
	CMsg* OnCreateMsg(int nMsgID);
	void OnMsg(SocketID fd, CMsg *pMsg);
	void OnClose(SocketID fd, const char *szIP, int nPort);

	CTcpServer::CTcpServer(): m_bStop(false), m_nServerType(0), m_lServerID(0), m_nPort(0)
	{
		memset(m_szServerName, 0, sizeof(m_szServerName));
		memset(m_arrServerPeer, 0, sizeof(m_arrServerPeer));
		memset(m_arrMaxServerID, 0, sizeof(m_arrMaxServerID));
		g_pTcpServer = this;
	}

	CTcpServer::~CTcpServer()
	{
		for (int i = 0; i < Max_ServerType_Count; i++)
		{
			for (int j = 0; j < Max_ServerID; j++)
			{
				SAFE_DELETE(m_arrServerPeer[i][j]);
			}
		}
		
		CHashMap<uint64, CPeer*>::HashNode *pNode = m_mapClientPeer.Begin();
		while (pNode != NULL)
		{
			SAFE_DELETE(pNode->m_Value);
			pNode = pNode->Next();
		}
	}

	bool CTcpServer::Init()
	{
		CRandom::SRand();
		//开启日志
		CLogger::Instance()->StartLog(m_szServerName);
		WriteLog(Log_Level_Info, "%s Start...", m_szServerName);
		//加载全局配置
		if (!LoadGlobalIni())
		{
			return false;
		}
		WriteLog(Log_Level_Info, "Load Global.ini OK");
		//加载本地配置
		if (!LoadLocalIni())
		{
			return false;
		}
		WriteLog(Log_Level_Info, "Load %s.ini OK", m_szServerName);
		CTimeScheduler<CPeer>::Instance()->Init();
		//启动网络线程
		CTcpManager::Instance()->RegisterOnConnect(Minicat::OnConnect);
		CTcpManager::Instance()->RegisterOnClose(Minicat::OnClose);
		CTcpManager::Instance()->RegisterOnMsg(Minicat::OnMsg);
		CTcpManager::Instance()->RegisterOnCreateMsg(Minicat::OnCreateMsg);
		if (!CTcpManager::Instance()->Init())
		{
			WriteLog(Log_Level_Error, "CTcpManager::Init Failure");
			return false;
		}
		WriteLog(Log_Level_Info, "CTcpManager Init OK");
		if (m_nPort > 0)
		{
			CTcpManager::Instance()->Listen(m_nPort);
		}
		OnStart();//交给上层处理
		WriteLog(Log_Level_Info, "%s Start OK", m_szServerName);
		Loop();
		return true;
	}

	void CTcpServer::StartDBThread()
	{
		const char *szSection = "Database";
		char szIP[Max_IP4_Len];
		char szDBUserName[Max_DBUserName];
		char szPwd[Max_DBPwd];
		char szDBName[Max_DBName];
		m_iniGlobal.ReadStr(szSection, "IP", szIP, sizeof(szIP));
		int nPort = m_iniGlobal.ReadInt(szSection, "Port", 3306);
		m_iniGlobal.ReadStr((char*)szSection, (char*)"UserName", szDBUserName, sizeof(szDBUserName));
		m_iniGlobal.ReadStr((char*)szSection, (char*)"Pwd", szPwd, sizeof(szPwd));
		m_iniGlobal.ReadStr((char*)szSection, (char*)"DBName", szDBName, sizeof(szDBName));

		CMysqlManager::Instance()->SetDBInfo(szIP, nPort, szDBUserName, szPwd, szDBName);
		CMysqlManager::Instance()->Init();
	}
	
	void CTcpServer::Loop()
	{
		while (!m_bStop)
		{
			CTimeClock::Update();
			CTcpManager::Instance()->ProcessEvent();
			TickPeerTimeout();
			OnTick();
			m_Fps.Tick();
		}
	}

	void CTcpServer::TickPeerTimeout()
	{
		static unsigned int nLoop = 0;
		if ((nLoop % GetFps()) == 0)
		{
			CTimeScheduler<CPeer>::Instance()->Tick();
		}
		nLoop++;
	}

	bool CTcpServer::LoadGlobalIni()
	{
		char szPath[Max_FullPath] = { 0 };
		GetCurrentPath(szPath, Max_FullPath);

		//设置日志文件路径
		char szIniFileName[Max_FullPath] = { 0 };
		sprintf(szIniFileName, "%s/Global.ini", szPath);
		//打开配置文件
		if (!m_iniGlobal.Open(szIniFileName))
		{
			WriteLog(Log_Level_Error, "%s Not Found", szIniFileName);
			return false;
		}
		return true;
	}

	bool CTcpServer::LoadLocalIni()
	{
		char szPath[Max_FullPath] = { 0 };
		GetCurrentPath(szPath, Max_FullPath);

		//设置日志文件路径
		char szIniFileName[Max_FullPath] = { 0 };
		sprintf(szIniFileName, "%s/%s.ini", szPath, m_szServerName);
		//打开配置文件
		if (!m_iniLocal.Open(szIniFileName))
		{
			WriteLog(Log_Level_Error, "%s Not Found", szIniFileName);
			return false;
		}

		m_lServerID = (int64)m_iniLocal.ReadInt((char*)"Settings", (char*)"ServerID", 0);
		if (m_lServerID <= 0)
		{
			WriteLog(Log_Level_Error, "ServerID not exist");
			return false;
		}
		m_nPort = m_iniLocal.ReadInt((char*)"Settings", (char*)"Port", 0);
		return true;
	}

	void CTcpServer::Connect(int nServerType, const char *szServerName)
	{
		if (m_nServerType <= PeerType_Unknow)
		{
			WriteLog(Log_Level_Error, "m_ServerType Is Not Set!");
			return;
		}
		for (uint64 lServerID = Min_ServerID; lServerID < Max_ServerID; lServerID++)
		{
			if(!Connect(nServerType, lServerID, szServerName))
			{
				break;
			}
		}
	}

	bool CTcpServer::Connect(int nServerType, uint64 lServerID, const char *szServerName)
	{
		if (m_nServerType <= PeerType_Unknow)
		{
			WriteLog(Log_Level_Error, "m_ServerType Is Not Set!");
			return false;
		}
		char szKey[8] = { 0 };
		sprintf(szKey, "%lld", lServerID);

		char szIPPort[32] = { 0 };
		if (!m_iniGlobal.ReadStr((char*)szServerName, szKey, szIPPort, sizeof(szIPPort)))
		{
			return false;
		}
		char szIP[Max_IP4_Len] = { 0 };
		int nPort = 0;
		if (!ParseIPPort(szIPPort, szIP, nPort))
		{
			WriteLog(Log_Level_Error, "%s: %d ParseIPPort return FALSE", __FILE__, __LINE__);
			return false;
		}
		CPeer *pPeer = new CPeer(nServerType, lServerID, szIP, nPort);
		m_arrServerPeer[nServerType][lServerID] = pPeer;
		SetMaxServerID(nServerType, lServerID);
		uint64 lKey = Hash(szIPPort);
		m_mapAddr2ServerPeer.Set(lKey, pPeer);
		CTcpManager::Instance()->Connect(szIP, nPort);
		return true;
	}

	void CTcpServer::SetMaxServerID(int nPeerType, uint64 lPeerID)
	{
		if (lPeerID > m_arrMaxServerID[nPeerType])
		{
			m_arrMaxServerID[nPeerType] = lPeerID;
		}
	}

	int CTcpServer::GetServerID_Mod(int nPeerType, uint64 lKey)
	{
		return (lKey % m_arrMaxServerID[nPeerType]) + 1;
	}

	int CTcpServer::GetServerID_Rand(int nPeerType)
	{
		return CRandom::Rand(m_arrMaxServerID[nPeerType]) + 1;
	}

	void CTcpServer::SetPeerInfo(SocketID fd, int nPeerType, uint64 lPeerID)
	{
		CPeer *pPeer = NULL;
		if (m_mapSocketID2Peer.Get(fd, pPeer))
		{
			pPeer->nPeerType = nPeerType;
			pPeer->lPeerID = lPeerID;

			if (nPeerType == PeerType_Client)
			{
				m_mapClientPeer.Set(lPeerID, pPeer);
			}
		}
	}

	void CTcpServer::Send2Server(int nServerType, uint64 lServerID, CMsg *pMsg)
	{
		CPeer *pPeer = m_arrServerPeer[nServerType][lServerID];
		if (pPeer)
		{
			CTcpManager::Instance()->SendMsg(pPeer->nSocketID, pMsg);
		}
	}

	void CTcpServer::Broadcast2Server(int nServerType, CMsg *pMsg)
	{
		uint64 lMaxServerID = m_arrMaxServerID[nServerType];
		for (uint64 lServerID = 1; lServerID <= lMaxServerID; lServerID++)
		{
			CPeer *pPeer = m_arrServerPeer[nServerType][lServerID];
			if (pPeer)
			{
				CTcpManager::Instance()->SendMsg(pPeer->nSocketID, pMsg);
			}
		}
	}

	void CTcpServer::Send2Client(uint64 lPeerID, CMsg *pMsg)
	{
		CPeer *pPeer = NULL;
		if (m_mapClientPeer.Get(lPeerID, pPeer))
		{
			CTcpManager::Instance()->SendMsg(pPeer->nSocketID, pMsg);
		}
	}

	void CTcpServer::CloseServer(int nServerType, uint64 lServerID)
	{
		CPeer *pPeer = m_arrServerPeer[nServerType][lServerID];
		if (pPeer)
		{
			CTcpManager::Instance()->Close(pPeer->nSocketID);
		}
	}

	void CTcpServer::CloseClient(uint64 lPeerID)
	{
		CPeer *pPeer = NULL;
		if (m_mapClientPeer.Get(lPeerID, pPeer))
		{
			CTcpManager::Instance()->Close(pPeer->nSocketID);
		}
	}

	void OnConnect(SocketID fd, const char *szIP, int nPort)
	{
		if (fd == INVALID_SOCKET)  //主动连接失败
		{
			WriteLog(Log_Level_Error, "Connect Fail %s %d", szIP, nPort);
			CTcpManager::Instance()->Connect(szIP, nPort);
			return;
		}

		char szIPPort[32] = { 0 };
		sprintf(szIPPort, "%s:%d", szIP, nPort);
		uint64 lKey = Hash(szIPPort);
		CPeer *pPeer = NULL;
		if (g_pTcpServer->m_mapAddr2ServerPeer.Get(lKey, pPeer))
		{
			//主动连接完成
			pPeer->nSocketID = fd;

			ServerConnectMsg msg;
			msg.nServerType = g_pTcpServer->GetServerType();
			msg.lServerID = g_pTcpServer->GetServerID();
			CTcpManager::Instance()->SendMsg(fd, &msg);
			//主动连接完成，通知上层
			g_pTcpServer->OnConnect(pPeer->nPeerType, pPeer->lPeerID);
		}
		else
		{
			//被动连接
			pPeer = new CPeer(PeerType_Unknow, 0, szIP, nPort);
			pPeer->nSocketID = fd;
			pPeer->SetTimeout(30);
		}
		g_pTcpServer->m_mapSocketID2Peer.Set(fd, pPeer);
	}

	CMsg* OnCreateMsg(int nMsgID)
	{
		if (nMsgID == Heartbeat_Msg)
		{
			return new HeartbeatMsg();
		}
		else if (nMsgID == ServerConnect_Msg)
		{
			return new ServerConnectMsg();
		}
		else
		{
			return g_pTcpServer->OnCreateMsg(nMsgID);
		}
	}

	void OnMsg(SocketID fd, CMsg *pMsg)
	{
		CPeer *pPeer = NULL;
		g_pTcpServer->m_mapSocketID2Peer.Get(fd, pPeer);
		if (!pPeer)
		{
			return;
		}
		int nMsgID = pMsg->MsgID();
		if (nMsgID == Heartbeat_Msg)
		{
			HeartbeatMsg *pHeartbeat = (HeartbeatMsg *)pMsg;
			if (pHeartbeat->nHeartbeat > 0)
			{
				pHeartbeat->nHeartbeat--;
				CTcpManager::Instance()->SendMsg(fd, pHeartbeat);
			}
			SAFE_DELETE(pMsg)
		}
		else if (nMsgID == ServerConnect_Msg)
		{
			ServerConnectMsg *pConnect = (ServerConnectMsg *)pMsg;
			
			CPeer *pOldPeer = g_pTcpServer->m_arrServerPeer[pConnect->nServerType][pConnect->lServerID];
			if (pOldPeer)
			{
				WriteLog(Log_Level_Info, "Conflict Peer ServerType=%d ServerID=%lld", pConnect->nServerType, pConnect->lServerID);
				CTcpManager::Instance()->Close(fd);
			}
			else
			{
				pPeer->nPeerType = pConnect->nServerType;
				pPeer->lPeerID = pConnect->lServerID;
				g_pTcpServer->m_arrServerPeer[pPeer->nPeerType][pPeer->lPeerID] = pPeer;
				g_pTcpServer->SetMaxServerID(pPeer->nPeerType, pPeer->lPeerID);
				//通知上层连接建立
				g_pTcpServer->OnConnect(pConnect->nServerType, pConnect->lServerID);
			}
			SAFE_DELETE(pMsg)
		}
		else
		{
			if (pPeer && pPeer->IsIdentified())
			{
				//上层处理
				if (!g_pTcpServer->OnMsg(pPeer->nPeerType, pPeer->lPeerID, pMsg))
				{
					CTcpManager::Instance()->Close(fd);
					WriteLog(Log_Level_Error, "OnMsg return FALSE fd=%d MsgID=%d", fd, pMsg->MsgID());
				}
			}
			else
			{
				//上层验证
				if (!g_pTcpServer->OnIdentify(fd, pMsg))
				{
					WriteLog(Log_Level_Error, "OnIdentify return FALSE fd=%d", fd, pMsg->MsgID());
					CTcpManager::Instance()->Close(fd);
				}
			}
			SAFE_DELETE(pMsg)
		}
		if (pPeer)
		{
			if (pPeer->nPeerType == PeerType_Client)
			{
				pPeer->SetTimeout(180);
			}
			else
			{
				pPeer->KillTimeout();
			}
		}
	}

	void OnClose(SocketID fd, const char *szIP, int nPort)
	{
		CPeer *pPeer = NULL;
		if (g_pTcpServer->m_mapSocketID2Peer.Get(fd, pPeer))
		{
			//连接断开
			g_pTcpServer->m_mapSocketID2Peer.Erase(fd);
			pPeer->KillTimeout();
			int nPeerType = pPeer->nPeerType;
			uint64 lPeerID = pPeer->lPeerID;
			SAFE_DELETE(pPeer)
			if (nPeerType >= 0 && nPeerType < Max_ServerType_Count)
			{
				g_pTcpServer->m_arrServerPeer[nPeerType][lPeerID] = NULL;
				g_pTcpServer->OnClose(nPeerType, lPeerID);
			}
			else if (nPeerType == PeerType_Client)
			{
				g_pTcpServer->m_mapClientPeer.Erase(lPeerID);
				g_pTcpServer->OnClose(nPeerType, lPeerID);
			}
		}
		else
		{
			//主动连接失败
			char szIPPort[32] = { 0 };
			sprintf(szIPPort, "%s:%d", szIP, nPort);
			uint64 lKey = Hash(szIPPort);
			CPeer *pPeer = NULL;
			if (g_pTcpServer->m_mapAddr2ServerPeer.Get(lKey, pPeer))
			{
				g_pTcpServer->m_mapAddr2ServerPeer.Erase(lKey);
				int nPeerType = pPeer->nPeerType;
				uint64 lPeerID = pPeer->lPeerID;
				SAFE_DELETE(g_pTcpServer->m_arrServerPeer[nPeerType][lPeerID])
				g_pTcpServer->OnClose(nPeerType, lPeerID);
			}
		}
	}
}