#include "MySqlManager.h"
#include "Logger.h"
namespace Minicat
{
	CMysqlManager::CMysqlManager()
	{
		memset(m_szIP, 0, sizeof(m_szIP));
		memset(m_szUserName, 0, sizeof(m_szUserName));
		memset(m_szPwd, 0, sizeof(m_szPwd));
		memset(m_szDBName, 0, sizeof(m_szDBName));
		m_nPort = 0;
		//由于该方法不能在多线程中调用，单独提出来
		mysql_library_init(0,NULL,NULL);
	}
	
	CMysqlManager::~CMysqlManager()
	{

	}

	void CMysqlManager::SetDBInfo(const char* szIP, int nPort, const char* szUserName, const char* szPwd, const char* szDBName)
	{
		strncpy(m_szIP, szIP, sizeof(m_szIP) - 1);
		strncpy(m_szUserName, szUserName, sizeof(m_szUserName) - 1);
		strncpy(m_szPwd, szPwd, sizeof(m_szPwd) - 1);
		strncpy(m_szDBName, szDBName, sizeof(m_szDBName) - 1);
		m_nPort = nPort;
	}

	void CMysqlManager::OnThreadStart(int nThreadIndex)
	{
		if(!m_MySqlConnector[nThreadIndex].Connect(m_szIP, m_nPort, m_szUserName, m_szPwd, m_szDBName))
		{
			WriteLog(Log_Level_Error, "Connect DB Fail %d ,IP=%s,Port=%d,DBName=%s,UserName=%s", nThreadIndex, m_szIP, m_nPort, m_szDBName, m_szUserName);
		}
		else
		{
			WriteLog(Log_Level_Info, "Connect DB Success %d,IP=%s,Port=%d,DBName=%s,UserName=%s", nThreadIndex, m_szIP, m_nPort, m_szDBName, m_szUserName);
		}
	}

	void CMysqlManager::OnExecute(int nThreadIndex, MySqlTask *pTask)
	{
		int nErrNo = 0;
		if (!m_MySqlConnector[nThreadIndex].Execute(pTask, nErrNo))
		{
			WriteLog(Log_Level_Error, "Sql Execute Fail ErrNo=%d, Sql=%s", nErrNo, pTask->m_szSql);
		}
		m_queueFinish.Push(pTask);
	}

	void CMysqlManager::Tick()
	{
		MySqlTask *pTask = NULL;
		while (m_queueFinish.Pop(pTask))
		{
			pTask->OnTaskFinish();
			SAFE_DELETE(pTask);
		}
	}
}