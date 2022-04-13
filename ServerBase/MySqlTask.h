#pragma once
#include "CommonBase.h"
#include "CommonDefine.h"
#include <mysql.h>
#include <stdio.h>
#include "ResultSet.h"
namespace Minicat
{
	class MySqlTask
	{
	public:
		enum
		{
			Max_Sql_Size = 4096
		};
	public:
		virtual void OnExecuteResult(CResultSet *pResultSet) {};
		virtual void OnTaskFinish() {};
	public:
		MySqlTask()
		{
			m_szSql = NULL;
			m_nSqlLen = 0;
		}

		virtual ~MySqlTask()
		{
			SAFE_FREE(m_szSql);
			
		}

		void SetSql(const char *szFormat, ...)
		{
			m_szSql = (char*)malloc(Max_Sql_Size);
			va_list ap;
			va_start(ap, szFormat);
			m_nSqlLen = vsprintf(m_szSql, szFormat, ap);
			va_end(ap);
		}
	public:
		char *m_szSql;
		int m_nSqlLen;
	};
}