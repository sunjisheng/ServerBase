#include "CommonBase.h"
#include "GlobalFunction.h"
#include "CommonDefine.h"
#include "TimeClock.h"
namespace Minicat
{
	uint64 GetTickCount()
	{
#if defined(_WINDOWS)
		return (uint64)::GetTickCount();
#elif defined(_LINUX)
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#endif
	}

	int GetFilePath(const char *szFileName, char *szPath, int nLen)
	{
		strncpy(szPath, szFileName, nLen);
#if defined(_WINDOWS)
		char *p = strrchr(szPath, '\\');
#elif defined(_LINUX)
		char *p = strrchr(szPath, '/');
#endif
		if (p != NULL)
		{
			*p = '\0';
		}
		return (p - szPath);
	}

	int GetCurrentPath(char *szPath, int nLen)
	{
#if defined(_WINDOWS)
		return GetFilePath(__argv[0], szPath, nLen);
#elif defined(_LINUX)
		char szFileName[Max_FullPath] = { 0 };
		readlink("/proc/self/exe", szFileName, Max_FullPath);
		return GetFilePath(szFileName, szPath, Max_FullPath);
#endif
	}

	int MyGetFileSize(char*szFileName)
	{
		try
		{
			struct stat filestat;
			int nRet = stat(szFileName, &filestat);
			if (nRet != -1)
			{
				return filestat.st_size;
			}
		}
		catch (...)
		{

		}
		return 0;
	}

	void Daemonize()
	{
#if defined(_LINUX)
		pid_t	pid;
		if ((pid = fork()) < 0)
		{
			exit(0);
		}
		else if (pid != 0)
		{
			exit(0);
		}
		fclose(stdin);
		fclose(stdout);
		fclose(stderr);
		setsid();
		signal(SIGHUP, SIG_IGN);
		if ((pid = fork()) != 0)
		{
			exit(0);
		}
		umask(0);
#endif
	}

	void MySleep(unsigned int nMS)
	{
#if defined(_WINDOWS)
		Sleep(nMS);
#elif defined(_LINUX)
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = nMS * 1000;
		select(0, NULL, NULL, NULL, &tv);
#endif
	}

	string GetCurrentThreadID()
	{
		stringstream ss;
		ss << std::this_thread::get_id();
		return ss.str();
	}

	bool ParseIPPort(const char *str, char *szIP, int& nPort)
	{
		while (*str != ',' && *str != ':' && *str != '\0')
		{
			*szIP++ = *str++;
		}
		*szIP = '\0';
		if (*str == '\0')
		{
			return false;
		}
		str++;
		nPort = atoi(str);
		return true;
	}

	uint64 Hash(const char *str)
	{
		register uint64 hash = 5381;
		while (*str) 
		{
			hash = ((hash << 5) + hash) + *str++;
		}
		return hash;
	}

	int64 GenUniqueID(int nServerID, int nObjectType)
	{
		static ushort nSerialNo = 0;
		nSerialNo = nSerialNo >= 9999 ? 1 : (++nSerialNo);
		int64 nID = (int64)CTimeClock::GetTime();
		nID = nID * 100 + nServerID;
		nID = nID * 100 + nObjectType;
		nID = nID * 10000 + nSerialNo;
		return nID;
	}

	bool IsIPAddr(const char *szIP)
	{
		int i = 0;
		do
		{
			char ch = szIP[i];
			if ((ch < '0' || ch > '9') && (ch != '.'))
				return false;

		} while (szIP[i++] != '\0');

		return true;
	}

	void ConvertDomain2IP(char *szDomain, char *szIP)
	{
		if (IsIPAddr(szDomain))
		{
			return;
		}

		struct hostent* h = NULL;
		if ((h = gethostbyname(szDomain)) == NULL)
		{
			return;
		}
		sprintf(szIP, "%s", inet_ntoa(*((struct in_addr *)h->h_addr)));
	}

	void ParseURL(char *szUrl, char *szHost, int HostLen, char *szDir, int nDirLen)
	{
		char *szStart = szUrl;
		char *szFind = strstr(szStart, "http://");
		if (szFind)
		{
			szStart = szFind + 7;
		}
		char *szDst = szHost;
		while (*szStart != NULL)
		{
			if (*szStart == '/')
			{
				szDst = szDir;
				break;
			}
			*szDst++ = *szStart++;
		}
		while (*szStart != NULL)
		{
			*szDst++ = *szStart++;
		}
	}

	void Hex2Str(char *szOut, const char *szInput, int nLen)
	{
		for (int i = 0; i < nLen; i++)
		{
			char byVal = ((szInput[i] & 0xf0) >> 4);
			szOut[i * 2] = (byVal >= 10 ? byVal - 10 + 'a' : byVal + '0');
			byVal = (szInput[i] & 0x0f);
			szOut[i * 2 + 1] = (byVal >= 10 ? byVal - 10 + 'a' : byVal + '0');
		}
	}

	void Str2Hex(char *szOut, const char *szInput, int nLen)
	{
		int nLenOut = nLen / 2;
		for (int i = 0; i < nLenOut; i++)
		{
			char high = (szInput[2 * i] >= 'a' ? szInput[2 * i] + 10 - 'a' : szInput[2 * i] - '0');
			char low = (szInput[2 * i + 1] >= 'a' ? szInput[2 * i + 1] + 10 - 'a' : szInput[2 * i + 1] - '0');
			szOut[i] = (((high & 0x0f) << 4) + low);
		}
	}

	void SplitStr(string &str, char sep, vector<string> &ve)
	{
		int start;
		int end = -1;
		do
		{
			start = end + 1;
			end = str.find(sep, start);
			if (end != string::npos)
			{
				ve.push_back(str.substr(start, end - start));
			}
		} while (end != string::npos);

		ve.push_back(str.substr(start));
	}

	int IsUtf8Byte(unsigned char c)
	{
		unsigned special_byte = 0X02; //binary 00000010    
		if (c >> 6 == special_byte) {
			return 1;
		}
		else {
			return 0;
		}
	}

	int IsUtf8(const char *str, int len)
	{
		unsigned one_byte = 0X00; //binary 00000000
		unsigned two_byte = 0X06; //binary 00000110
		unsigned three_byte = 0X0E; //binary 00001110  
		unsigned four_byte = 0X1E; //binary 00011110
		unsigned five_byte = 0X3E; //binary 00111110
		unsigned six_byte = 0X7E; //binary 01111110

		int utf8_yes = 0;
		int utf8_no = 0;

		unsigned char k = 0;
		unsigned char m = 0;
		unsigned char n = 0;
		unsigned char p = 0;
		unsigned char q = 0;

		unsigned char c = 0;

		for (int i = 0; i < len;)
		{
			c = (unsigned char)str[i];
			if (c >> 7 == one_byte)
			{
				i++;
				continue;
			}
			else if (c >> 5 == two_byte)
			{
				k = (unsigned char)str[i + 1];
				if (IsUtf8Byte(k)) {
					utf8_yes++;
					i += 2;
					continue;
				}
			}
			else if (c >> 4 == three_byte)
			{
				m = (unsigned char)str[i + 1];
				n = (unsigned char)str[i + 2];
				if (IsUtf8Byte(m)
					&& IsUtf8Byte(n))
				{
					utf8_yes++;
					i += 3;
					continue;
				}
			}
			else if (c >> 3 == four_byte)
			{
				k = (unsigned char)str[i + 1];
				m = (unsigned char)str[i + 2];
				n = (unsigned char)str[i + 3];
				if (IsUtf8Byte(k)
					&& IsUtf8Byte(m)
					&& IsUtf8Byte(n))
				{
					utf8_yes++;
					i += 4;
					continue;
				}
			}
			else if (c >> 2 == five_byte)
			{
				unsigned char k = (unsigned char)str[i + 1];
				unsigned char m = (unsigned char)str[i + 2];
				unsigned char n = (unsigned char)str[i + 3];
				unsigned char p = (unsigned char)str[i + 4];
				if (IsUtf8Byte(k)
					&& IsUtf8Byte(m)
					&& IsUtf8Byte(n)
					&& IsUtf8Byte(p))
				{
					utf8_yes++;
					i += 5;
					continue;
				}
			}
			else if (c >> 1 == six_byte)
			{
				k = (unsigned char)str[i + 1];
				m = (unsigned char)str[i + 2];
				n = (unsigned char)str[i + 3];
				p = (unsigned char)str[i + 4];
				q = (unsigned char)str[i + 5];
				if (IsUtf8Byte(k)
					&& IsUtf8Byte(m)
					&& IsUtf8Byte(n)
					&& IsUtf8Byte(p)
					&& IsUtf8Byte(q)) {
					utf8_yes++;
					i += 6;
					continue;
				}
			}
			utf8_no++;
			i++;
		}
		if (utf8_yes == 0 && utf8_no == 0)
		{
			return 1;
		}
		int ret = (100 * utf8_yes) / (utf8_yes + utf8_no);
		if (ret > 90)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

