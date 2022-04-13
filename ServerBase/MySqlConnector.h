#pragma once
/************************************************************************/
/* MySQL访问工具类                                                        */
/************************************************************************/
#include <mysql.h>
#include "MySqlTask.h"
namespace Minicat
{
	class CMySqlConnector
	{
	public:
		CMySqlConnector();
		virtual ~CMySqlConnector();

	public:
		//打开mysql
		bool Connect(const char* strIP, int nPort, const char* strUsr, const char* strPwd, const char* strDBName);
		//关闭mysql
		void Close();
		//执行一次查询
		bool Execute(MySqlTask *pTask, int &nErrNo);
		void Clear();
	private:
		MYSQL*	m_hMysql;
	};
}