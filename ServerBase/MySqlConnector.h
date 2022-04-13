#pragma once
/************************************************************************/
/* MySQL���ʹ�����                                                        */
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
		//��mysql
		bool Connect(const char* strIP, int nPort, const char* strUsr, const char* strPwd, const char* strDBName);
		//�ر�mysql
		void Close();
		//ִ��һ�β�ѯ
		bool Execute(MySqlTask *pTask, int &nErrNo);
		void Clear();
	private:
		MYSQL*	m_hMysql;
	};
}