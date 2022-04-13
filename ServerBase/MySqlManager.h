#pragma once
#include "ThreadPool.h"
#include "MySqlTask.h"
#include "MySqlConnector.h"
#include "NetBase.h"
#include "Singleton.h"
#include "TSQueue.h"
namespace Minicat
{
	enum
	{
		DB_Thread_Count = 5,
		Max_DBUserName = 32,
		Max_DBPwd = 32,
		Max_DBName = 32,
	};
	class CMysqlManager : public CThreadPool<MySqlTask, DB_Thread_Count>, public Singleton<CMysqlManager>
	{
	public:
		CMysqlManager();
		~CMysqlManager();
	public:
		virtual void OnThreadStart(int nThreadIndex);
		virtual void OnExecute(int nThreadIndex, MySqlTask *pTask);
	public:
		void SetDBInfo(const char* szIP, int nPort, const char* szUserName, const char* szPwd, const char* szDBName);
		void Tick(); //在主线程中调用
	private:
		char m_szIP[Max_IP4_Len];
		int m_nPort;
		char m_szUserName[Max_DBUserName];
		char m_szPwd[Max_DBPwd];
		char m_szDBName[Max_DBName];
		CMySqlConnector m_MySqlConnector[DB_Thread_Count];
		CTSQueue<MySqlTask*> m_queueFinish;
	};
}