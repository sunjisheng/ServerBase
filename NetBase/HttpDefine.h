#pragma once
#include "CommonBase.h"
#include "NetBase.h"
#include "GlobalFunction.h"
namespace Minicat
{
	enum
	{
		Max_Header_Count = 20,
		Max_Method_Len = 10,
		Max_URL_Len = 256,
		Max_Domain_Len = 32,
		Max_Directory_Len = 64,
		Max_Host_Len = 32,
		Max_HeaderName_Len = 20,
		Max_HeaderValue_Len = 128,
		Max_Content_Len = 4096,
		Max_Code_Len = 10,
	};

	enum HttpAct_ID
	{
		HttpAct_Listen = 1,
		HttpAct_Request = 2,
		HttpAct_Response = 3,
	};

	struct HttpHeader
	{
		HttpHeader()
		{
			memset(szName, 0, sizeof(szName));
			memset(szValue, 0, sizeof(szValue));
		}
		char szName[Max_HeaderName_Len];
		char szValue[Max_HeaderValue_Len];
	};

	struct HttpParamMap
	{
		map<string, string> m_mapParam;
		string GetParam(string szName)
		{
			map<string, string>::iterator it = m_mapParam.find(szName);
			if (it != m_mapParam.end())
			{
				return it->second;
			}
			return "";
		}
	};

	class HttpBody
	{
	public:
		HttpBody():nHeaderCount(0), nContentLen(0)
		{
			memset(szContent, 0, sizeof(szContent));
		}
		

		bool IsValid()
		{
			if (nHeaderCount || nContentLen)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		char* GetHeaderValue(char *szName)
		{
			for (int i = 0; i < nHeaderCount; i++)
			{
				if (!strcmp(szName, arrHeaders[i].szName))
				{
					return arrHeaders[i].szValue;
				}
			}
			return NULL;
		}

		void AddHeader(char *szName, char *szValue)
		{
			strncpy(arrHeaders[nHeaderCount].szName, szName, sizeof(arrHeaders[nHeaderCount].szName) - 1);
			strncpy(arrHeaders[nHeaderCount].szValue, szValue, sizeof(arrHeaders[nHeaderCount].szValue) - 1);
			nHeaderCount++;
		}

		void SetContent(const char *szFormat, ...)
		{
			va_list ap;
			va_start(ap, szFormat);
			nContentLen = vsprintf(szContent, szFormat, ap);
			va_end(ap);
		}

		char* GetContent()
		{
			return szContent;
		}

		void SplitContent(HttpParamMap &mapParams)
		{
			std::vector<string> ve;
			string str(szContent);
			SplitStr(str, '&', ve);
			for (int i = 0; i < ve.size(); i++)
			{
				std::vector<string> name_value;
				SplitStr(ve[i], '=', name_value);
				if (name_value.size() == 2)
				{
					mapParams.m_mapParam.insert(make_pair(name_value[0], name_value[1]));
				}
			}
		}
		int nHeaderCount;
		HttpHeader arrHeaders[Max_Header_Count];
		int nContentLen;
		char szContent[Max_Content_Len];
	};

	class HttpRequest : public HttpBody
	{
	public:
		HttpRequest()
		{
			memset(szDir, 0, sizeof(szDir));
		}
		void SetDir(const char *_szDir)
		{
			strncpy(szDir, _szDir, sizeof(szDir) - 1);
		}

		char *GetDir()
		{
			return szDir;
		}
	public:
		char szDir[Max_Directory_Len];
	};

	class HttpResponse : public HttpBody
	{
	public:
		HttpResponse()
		{
			memset(szCode, 0, sizeof(szCode));
		}
	public:
		char szCode[Max_Code_Len];
	};

	struct HttpReqAct
	{
		HttpReqAct() :nActID(0), pRequest(NULL)
		{
			memset(szUrl, 0, sizeof(szUrl));
		}
		int nActID;
		char szUrl[Max_URL_Len];
		HttpRequest *pRequest;
	};

	struct HttpAckAct
	{
		HttpAckAct() : nActID(0), nSocketID(INVALID_SOCKET)
		{
		}
		int nActID;
		SocketID nSocketID;
	};

	struct HttpEvent
	{
		HttpEvent() :nSocketID(0), pRequest(NULL), pResponse(NULL)
		{

		}
		SocketID nSocketID;
		HttpRequest *pRequest;
		HttpResponse *pResponse;
	};

	typedef void(*HttpRequestHandler)(SocketID fd, HttpRequest *pRequest);
	typedef void(*HttpResponseHandler)(HttpRequest *pRequest, HttpResponse *pResponse);
}
