/*********************************************************************************
  *����:  SunJiSheng
  *����:  2020/04/15
  *����:  ��־��
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
		bool CheckDayChange();                     //���ڸı�
		bool CheckFileHuge(int nLevel);            //�ļ�����ָ����С
		void CreateFile(int nLevel);               //�����ļ�
		void WriteFile(int nLevel);                //д�ļ�
		void FlushFile(int nLevel);                //ͬ����Ӳ��  
	private:
		int m_nMinLevel;							//��־�ȼ� 
		int m_nLastDay;                             //�������
		char m_szLoggerName[Max_Logger_Name];		//��־��
		char m_szCurrentPath[Max_FullPath];         //��ǰ·��
		CCircleQueue m_arrQueue[Log_Level_Count];	//������
		LogFileInfo m_arrFileInfo[Log_Level_Count]; //�ļ���Ϣ
		bool m_arrDirty[Log_Level_Count];           //�ļ��ͱ��
	};
}