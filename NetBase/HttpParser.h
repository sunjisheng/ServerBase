#pragma once
#include "HttpDefine.h"

namespace Minicat
{
	enum Http_Parse_State
	{
		Status_Start,
		Status_Dir,
		Status_Version,
		Status_Code,
		Status_CodeDesc,
		Status_Name,
		Status_Value,
		Status_Content,
	};

	enum
	{
		Parse_OK,
		Parse_Pending,
		Parse_Fail,
	};

	class CHttpParser
	{
	public:
		CHttpParser();
		void Reset();
		int  ParseRequest(HttpRequest *pMsg, CCircleQueue &queue);
		int  ParseResponse(HttpResponse *pMsg, CCircleQueue &queue);
	private:
		int ConsumeRequest(HttpRequest *pMsg, CCircleQueue &queue);
		int ConsumeResponse(HttpResponse *pMsg, CCircleQueue &queue);
		int ReadMethod(CCircleQueue &queue);
		int ReadDir(HttpRequest *pMsg, CCircleQueue &queue);
		int ReadVersion_Req(CCircleQueue &queue);
		int ReadVersion_Ack(CCircleQueue &queue);
		int ReadCode(HttpResponse *pResponse, CCircleQueue &queue);
		int ReadCodeDesc(HttpResponse *pResponse, CCircleQueue &queue);
		int ReadHeaderName(HttpBody *pMsg, CCircleQueue &queue);
		int ReadHeaderValue(HttpBody *pMsg, CCircleQueue &queue);
		int ReadContent(HttpBody *pMsg, CCircleQueue &queue);
		inline bool GetNext(CCircleQueue &queue);
		void ParseGetParam(HttpRequest *pMsg);
		static bool Is_Char(int c);
		static bool Is_Ctrl(int c);
		static bool Is_Special(int c);
		static bool Is_Digit(int c);
	private:
		int m_nStatus;
		char m_Current;
		char m_szMethod[Max_Method_Len];
		int m_nContentLen;
	};

	bool CHttpParser::GetNext(CCircleQueue &queue)
	{
		if (queue.Read((char*)&m_Current, 1))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
