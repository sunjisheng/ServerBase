/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  日志类
**********************************************************************************/
#pragma once
#include "Thread.h"
#include "CommonDefine.h"
#include "Singleton.h"
#include "CircleQueue.h"

namespace Minicat
{
	enum LogLevel
	{
		Log_Level_Debug = 0,
		Log_Level_Info = 1,
		Log_Level_Error = 2,
		Log_Level_Count,
	};

	void WriteLog(int nLevel, const char * szFormat, ...);


	class CLogger : public CThread, public Singleton<CLogger>
	{
	public:
		enum
		{
			Log_Line_Size = 1024,
			Log_Buffer_Size = 65535,
		};
		struct LogFileInfo
		{
			LogFileInfo():fp(nullptr), nSerialNo(1)
			{
				memset(szFileName, 0, sizeof(szFileName));
			}
			char szFileName[Max_FullPath];
			int nSerialNo;
			FILE* fp;
		};
	public:
		CLogger();
		~CLogger();
		void StartLog(const char *szLoggerName, int nMinLevel = 0);
		void WriteLog(int nLevel, const char *szContent, int nLen);
		virtual void Run();
	protected:
		bool CheckDayChange();                     //日期改变
		bool CheckFileHuge(int nLevel);            //文件超过指定大小
		void CreateFile(int nLevel);               //创建文件
		void WriteFile(int nLevel);                //写文件
		void FlushFile(int nLevel);                //同步到硬盘  
	private:
		int m_nMinLevel;							//日志等级 
		int m_nLastDay;                             //最后日期
		char m_szLoggerName[Max_Logger_Name];		//日志名
		char m_szCurrentPath[Max_FullPath];         //当前路径
		CCircleQueue m_arrQueue[Log_Level_Count];	//缓冲区
		LogFileInfo m_arrFileInfo[Log_Level_Count]; //文件信息
		bool m_arrDirty[Log_Level_Count];           //文件胀标记
	};
}