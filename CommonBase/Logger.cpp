#include "Logger.h"
#include "stdio.h"
#include "GlobalFunction.h"
#include "TimeClock.h"
namespace Minicat
{
	CLogger::CLogger() : m_nMinLevel(0), m_nLastDay(0)
	{
		memset(m_szLoggerName, 0, sizeof(m_szLoggerName));
		memset(m_szCurrentPath, 0, sizeof(m_szCurrentPath));
		memset(m_arrDirty, 0, sizeof(m_arrDirty));
		for (int nLevel = 0; nLevel < Log_Level_Count; nLevel++)
		{
			m_arrQueue[nLevel].Init(Log_Buffer_Size);
		}
	}

	CLogger::~CLogger()
	{
		for (int nLevel = 0; nLevel < Log_Level_Count; nLevel++)
		{
			if (m_arrFileInfo[nLevel].fp)
			{
				fclose(m_arrFileInfo[nLevel].fp);
			}
		}
	}
	
	void CLogger::StartLog(const char *szLoggerName, int nMinLevel)
	{
		strncpy(m_szLoggerName, szLoggerName, sizeof(m_szLoggerName) - 1);
		m_nMinLevel = nMinLevel;
		GetCurrentPath(m_szCurrentPath, sizeof(m_szCurrentPath) - 1);
		CThread::Start();
	}

	bool CLogger::CheckDayChange()
	{
		CTimeClock::GetTime();
		int nCurDay = CTimeClock::GetDay();
		if (nCurDay == m_nLastDay)
		{
			return false;
		}
		for (int nLevel = 0; nLevel < Log_Level_Count; nLevel++)
		{
			m_arrFileInfo[nLevel].nSerialNo = 1;
			if (m_arrFileInfo[nLevel].fp)
			{
				fclose(m_arrFileInfo[nLevel].fp);
				m_arrFileInfo[nLevel].fp = NULL;
			}
		}
		m_nLastDay = nCurDay;
		return true;
	}
	
	bool CLogger::CheckFileHuge(int nLevel)
	{
		if (MyGetFileSize(m_arrFileInfo[nLevel].szFileName) < 10485760)
		{
			return false;
		}
		m_arrFileInfo[nLevel].nSerialNo++;
		if (m_arrFileInfo[nLevel].fp)
		{
			fclose(m_arrFileInfo[nLevel].fp);
			m_arrFileInfo[nLevel].fp = NULL;
		}
		return true;
	}

	void CLogger::CreateFile(int nLevel)
	{
		static const char* g_szLevelName[Log_Level_Count] = { "Debug","Info","Error" };
		char szDate[20] = { 0 };
		sprintf(szDate, "%d%d%d", CTimeClock::GetYear(), CTimeClock::GetMonth(), CTimeClock::GetDay());
		sprintf(m_arrFileInfo[nLevel].szFileName, "%s/%s_%s_%s_%d.log", m_szCurrentPath, m_szLoggerName, g_szLevelName[nLevel], szDate, m_arrFileInfo[nLevel].nSerialNo);

		if (m_arrFileInfo[nLevel].fp)
		{
			fclose(m_arrFileInfo[nLevel].fp);
		}
		m_arrFileInfo[nLevel].fp = fopen(m_arrFileInfo[nLevel].szFileName, "a+b");
	}

	void CLogger::WriteLog(int nLevel, const char *szContent, int nLen)
	{
		if (nLevel < m_nMinLevel || nLevel >= Log_Level_Count)
		{
			return;
		}
		m_arrQueue[nLevel].Write(szContent, nLen);
	}

	void CLogger::Run()
	{
		static unsigned long long loop = 0;
		while (!m_bStop)
		{
			//¶¨ÆÚ¼ì²é
			if (loop % 100 == 0)
			{
				CheckDayChange();
				for (int nLevel = 0; nLevel < Log_Level_Count; nLevel++)
				{
					CheckFileHuge(nLevel);
				}
			}
			for (int nLevel = 0; nLevel < Log_Level_Count; nLevel++)
			{
				WriteFile(nLevel);
				FlushFile(nLevel);
			}
			MySleep(50);
			loop++;
		}
	}

	void CLogger::WriteFile(int nLevel)
	{
		int nLen = m_arrQueue[nLevel].GetReadLen();
		if (nLen > 0)
		{
			if (!m_arrFileInfo[nLevel].fp)
			{
				CreateFile(nLevel);
			}
		}
		while (nLen > 0)
		{
			fwrite(m_arrQueue[nLevel].GetReadPtr(), 1, nLen, m_arrFileInfo[nLevel].fp);
			m_arrQueue[nLevel].OnReaded(nLen);
			nLen = m_arrQueue[nLevel].GetReadLen();
			m_arrDirty[nLevel] = true;
		}
	}

	void CLogger::FlushFile(int nLevel)
	{
		if (!m_arrDirty[nLevel])
		{
			return;
		}
		m_arrDirty[nLevel] = false;
		if (m_arrFileInfo[nLevel].fp)
		{
			fflush(m_arrFileInfo[nLevel].fp);
		}
	}

	void WriteLog(int nLevel, const char *szFormat, ...)
	{
		char szContent[CLogger::Log_Line_Size] = { 0 };
		CTimeClock::GetTime();
		int nTimeLen = sprintf(szContent, "[%04d-%02d-%02d %02d:%02d:%02d]",
			CTimeClock::GetYear(), CTimeClock::GetMonth(), CTimeClock::GetDay(),
			CTimeClock::GetHour(), CTimeClock::GetMinute(), CTimeClock::GetSecond());

		va_list arg;
		va_start(arg, szFormat);
		int nLen = vsnprintf(&szContent[nTimeLen], sizeof(szContent) / sizeof(char) - nTimeLen - 1, szFormat, arg);
		va_end(arg);
		if (nLen < 0)
		{
			return;
		}
		nLen += nTimeLen;
		szContent[nLen++] = '\n';
		CLogger::Instance()->WriteLog(nLevel, szContent, nLen);

#if defined(_WINDOWS)
		printf("%s", szContent);
#endif
	}
}