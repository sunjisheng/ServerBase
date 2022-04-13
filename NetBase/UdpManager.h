/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  单线程可靠Udp实现
**********************************************************************************/
#pragma once
#include "Thread.h"
#include "Singleton.h"
#include "SocketPoll.h"
#include "UdpSocket.h"
#include "CircleQueue.h"
#include "NetBase.h"
#include "HashMap.h"
#include "Listener.h"
#include "List.h"
#include "Vector.h"
#include "SocketFactory.h"
#include "UdpProto.h"
#include "BufferPool.h"
#include "SocketPair.h"
namespace Minicat
{
	enum Udp_Const_Def
	{
		Max_UdpSocket_Count = 65535, //最大连接数
		Max_Package_Size = 65536,    //Udp数据包最大长度
		Max_MTU = 1400,				 //最大传输单元，1400保证不会分片
	};

	enum ReadSet_Index
	{
		ReadSet_Bak = 0,
		ReadSet_Cur = 1,
		ReadSet_Count = 2,
	};

	enum Poll_Result
	{
		PollResult_Error,
		PollResult_Event,
		PollResult_Timeout
	};
	class CUdpManager : public CThread, public Singleton<CUdpManager>
	{
		friend class CUdpSocket;
	public:
		CUdpManager();
		~CUdpManager();
	public:
		virtual void Run();									//线程函数
	public:
		//对外接口
		bool Init();										//启动线程
		bool Listen(int nPort);
		bool Connect(const char *szIP, int nPort);
		bool Close(SocketID nSocketID);
		bool Send(SocketID nSocketID, char *szBuf, int nLen);
		bool RecvByLen(SocketID nSocketID, char *szBuf, int nLen);			//接收指定长度
		int Recv(SocketID nSocketID, char *szBuf, int nLen);				//接收全部
		void RegisterOnConnect(ConnectHandler fnOnConnect);
		void RegisterOnMsg(MsgHandler fnOnMsg);
		void RegisterOnClose(CloseHandler fnOnClose);
		void NotifyAct();
		void ProcessEvent();
	protected:
		int Poll(int nMS);  //事件轮询
		void ProcessAct();
		bool InnerListen(int nPort);
		bool InnerConnect(const char *szIP, int nPort);
		void InnerSend(SocketID nSocketID);
		bool InnerClose(SocketID nSocketID);

		bool PushEvent(int nEventID, SocketID nSocketID);
		bool PushConnectEvent(int nEventID, SocketID nSocketID, const char *szIP, int nPort);

		bool AddSocket(CUdpSocket *pSocket);
		bool DelSocket(CUdpSocket *pSocket);
		CUdpSocket* GetSocket(SocketID nSocketID);

		void OnPacket(UdpHeader *pPacket, sockaddr_in &addrPeer);
	public:
		CBufferPool m_BufferPool;                       //缓冲池,用于分配不定长的包缓存
	protected:
		SocketID m_nSocketID;							//Udp Socket句柄
		fd_set	m_ReadSet[ReadSet_Count];				//Select读集合
		SocketID m_fdMax;
		ConnectHandler m_fnOnConnect;					//连接回调
		MsgHandler m_fnOnMsg;                           //接收回调
		CloseHandler m_fnOnClose;						//关闭回调
		CSocketPair *m_pSocketPair;                     // 
		CCircleQueue m_queueAct;						//上层行为队列	
		CCircleQueue m_queueEvent;						//向上层通知事件队列
		CUdpSocket* m_pSockets[Max_UdpSocket_Count];    //Socket数组
		bool m_arrBusyPos[Max_UdpSocket_Count];         //已经被占用的
		int m_nFreePos;  //空闲位置                       //空闲位置
		char m_szRecvBuf[Max_Package_Size];				//Udp接收缓冲区
	};
}