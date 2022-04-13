#include "CommonBase.h"
#include "MySqlConnector.h"
#include <errmsg.h>
#include "Logger.h"
#include "ResultSet.h"
namespace Minicat
{
	CMySqlConnector::CMySqlConnector()
	{
		m_hMysql = NULL;
	}

	CMySqlConnector::~CMySqlConnector()
	{
		if (m_hMysql != NULL)
		{
			mysql_close(m_hMysql);
		}
	}
	
	bool CMySqlConnector::Connect(const char* strIP, int nPort, const char* strUsr, const char* strPwd, const char* strDBName)
	{
		m_hMysql = mysql_init(NULL);
		if (m_hMysql == NULL)
		{
			WriteLog(Log_Level_Error, "mysql_init return NULL");
			return false;
		}

		if (strIP == NULL || strUsr == NULL || strPwd == NULL || strDBName == NULL) 
		{
			WriteLog(Log_Level_Error, "CMySqlConnector::Connect Param NULL");
			return false;
		}

		if (!mysql_real_connect(m_hMysql, strIP, strUsr, strPwd, strDBName, nPort, NULL, CLIENT_MULTI_RESULTS)) 
		{
			int nerror = mysql_errno(m_hMysql);
			char* errorstr = (char*)mysql_error(m_hMysql);
			WriteLog(Log_Level_Error, "mysql_real_connect return %d %s", nerror, errorstr);
			return false;
		}
		char value = 1;
		mysql_options(m_hMysql, MYSQL_OPT_RECONNECT, (const void*)&value);
		return true;
	}

	void CMySqlConnector::Close()
	{
		if (m_hMysql)
		{
			mysql_close(m_hMysql);
			m_hMysql = NULL;
		}
	}

	bool CMySqlConnector::Execute(MySqlTask *pTask, int &nErrNo)
	{
		if (!m_hMysql)
		{
			WriteLog(Log_Level_Error, "CMySqlConnector::Execute m_hMysql is NULL");
			return false;
		}
		int nRet = mysql_real_query(m_hMysql, pTask->m_szSql, pTask->m_nSqlLen);
		if (nRet)
		{
			nErrNo = mysql_errno(m_hMysql);
			WriteLog(Log_Level_Error, "CMySqlConnector::Execute error %d", nErrNo);
			return false;
		}
		CResultSet result;
		result.m_pResult = mysql_store_result(m_hMysql);
		pTask->OnExecuteResult(&result);
		Clear();
		return true;
	}

	void CMySqlConnector::Clear()
	{
		while (mysql_more_results(m_hMysql))
		{
			mysql_next_result(m_hMysql);
		}
	}

}