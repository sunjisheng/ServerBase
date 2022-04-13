#include "HttpParser.h"

namespace Minicat
{
	CHttpParser::CHttpParser()
	{
		Reset();
	}

	void CHttpParser::Reset()
	{
		m_nStatus = Status_Start;
		m_Current = 0;
		memset(m_szMethod, 0, sizeof(m_szMethod));
		m_nContentLen = 0;
	}

	int CHttpParser::ParseRequest(HttpRequest *pMsg, CCircleQueue &queue)
	{
		if (queue.IsEmpty())
		{
			return Parse_Fail;
		}
		int result = Parse_Pending;
		while (!queue.IsEmpty())
		{
			result = ConsumeRequest(pMsg, queue);
			if (result == Parse_Fail || result == Parse_OK)
			{
				break;
			}
		}
		return result;
	}

	int CHttpParser::ParseResponse(HttpResponse *pMsg, CCircleQueue &queue)
	{
		if (queue.IsEmpty())
		{
			return Parse_Fail;
		}
		int result = Parse_Pending;
		while (!queue.IsEmpty())
		{
			result = ConsumeResponse(pMsg, queue);
			if (result == Parse_Fail || result == Parse_OK)
			{
				break;
			}
		}
		return result;
	}

	int CHttpParser::ConsumeRequest(HttpRequest *pMsg, CCircleQueue &queue)
	{
		switch (m_nStatus)
		{
		case Status_Start:
			return ReadMethod(queue);
		case Status_Dir:
			return ReadDir(pMsg, queue);
		case Status_Version:
			return ReadVersion_Req(queue);
		case Status_Name:
			return ReadHeaderName(pMsg, queue);
		case Status_Value:
			return ReadHeaderValue(pMsg, queue);
		case Status_Content:
			return ReadContent(pMsg, queue);
		}
		return Parse_Fail;
	}

	int CHttpParser::ConsumeResponse(HttpResponse *pResponse, CCircleQueue &queue)
	{
		switch (m_nStatus)
		{
		case Status_Start:
			return ReadVersion_Ack(queue);
		case Status_Code:
			return ReadCode(pResponse, queue);
		case Status_CodeDesc:
			return ReadCodeDesc(pResponse, queue);
		case Status_Name:
			return ReadHeaderName(pResponse, queue);
		case Status_Value:
			return ReadHeaderValue(pResponse, queue);
		case Status_Content:
			return ReadContent(pResponse, queue);
		}
		return Parse_Fail;
	}

	int CHttpParser::ReadMethod(CCircleQueue &queue)
	{
		int nLen = 0;
		while (GetNext(queue))
		{
			if (m_Current == ' ')
			{
				m_nStatus = Status_Dir;
				return Parse_Pending;
			}
			else if (!Is_Char(m_Current) || Is_Ctrl(m_Current) || Is_Special(m_Current))
			{
				return Parse_Fail;
			}
			else
			{
				m_szMethod[nLen++] = m_Current;
			}
		}
		return Parse_Pending;
	}

	int CHttpParser::ReadDir(HttpRequest *pRequest, CCircleQueue &queue)
	{
		int nLen = 0;
		while (GetNext(queue))
		{
			if (m_Current == ' ')
			{
				ParseGetParam(pRequest);
				m_nStatus = Status_Version;
				return Parse_Pending;
			}
			else if (Is_Ctrl(m_Current))
			{
				return Parse_Fail;
			}
			else
			{
				pRequest->szDir[nLen++] = m_Current;
			}
		}
		return Parse_Pending;
	}

	void CHttpParser::ParseGetParam(HttpRequest *pRequest)
	{
		char *szPos = strchr(pRequest->szDir, '?');
		if (szPos != NULL)
		{
			*szPos++ = '\0';
			strncpy(pRequest->szContent, szPos, sizeof(pRequest->szContent) - 1);
			pRequest->nContentLen = strlen(pRequest->szContent);
		}
	}

	int CHttpParser::ReadVersion_Req(CCircleQueue &queue)
	{
		int nLen = 0;
		while (GetNext(queue))
		{
			if (m_Current == '\r')
			{
				GetNext(queue);
				if (m_Current == '\n')
				{
					m_nStatus = Status_Name;
					return Parse_Pending;
				}
				else
				{
					return Parse_Fail;
				}
			}
			else if (Is_Ctrl(m_Current))
			{
				return Parse_Fail;
			}
		}
		return Parse_Pending;
	}

	int CHttpParser::ReadVersion_Ack(CCircleQueue &queue)
	{
		int nLen = 0;
		while (GetNext(queue))
		{
			if (m_Current == ' ')
			{
				m_nStatus = Status_Code;
				return Parse_Pending;
			}
			else if (Is_Ctrl(m_Current))
			{
				return Parse_Fail;
			}
		}
		return Parse_Pending;
	}

	int CHttpParser::ReadCode(HttpResponse *pResponse, CCircleQueue &queue)
	{
		int nLen = 0;
		while (GetNext(queue))
		{
			if (m_Current == ' ')
			{
				m_nStatus = Status_CodeDesc;
				return Parse_Pending;
			}
			else if (!Is_Char(m_Current) || Is_Ctrl(m_Current) || Is_Special(m_Current))
			{
				return Parse_Fail;
			}
			else
			{
				pResponse->szCode[nLen++] = m_Current;
			}
		}
		return Parse_Pending;
	}

	int CHttpParser::ReadCodeDesc(HttpResponse *pResponse, CCircleQueue &queue)
	{
		int nLen = 0;
		while (GetNext(queue))
		{
			if (m_Current == '\r')
			{
				GetNext(queue);
				if (m_Current == '\n')
				{
					m_nStatus = Status_Name;
					return Parse_Pending;
				}
				else
				{
					return Parse_Fail;
				}
			}
			else if (!Is_Char(m_Current) || Is_Ctrl(m_Current) || Is_Special(m_Current))
			{
				return Parse_Fail;
			}
		}
		return Parse_Pending;
	}

	int CHttpParser::ReadHeaderName(HttpBody *pMsg, CCircleQueue &queue)
	{
		int nLen = 0;
		while (GetNext(queue))
		{
			if (m_Current == '\r')
			{
				GetNext(queue);
				if (m_Current == '\n')
				{
					if (m_nContentLen > 0)
					{
						m_nStatus = Status_Content;
						return Parse_Pending;
					}
					else
					{
						return Parse_OK;
					}
				}
				else
				{
					return Parse_Fail;
				}
			}
			else if (m_Current == ':')
			{
				GetNext(queue);
				if (m_Current == ' ')
				{
					m_nStatus = Status_Value;
				}
				else
				{
					return Parse_Fail;
				}
				return Parse_Pending;
			}
			else if (!Is_Char(m_Current) || Is_Ctrl(m_Current) || Is_Special(m_Current))
			{
				return Parse_Fail;
			}
			else
			{
				pMsg->arrHeaders[pMsg->nHeaderCount].szName[nLen++] = m_Current;
			}
		}
		return Parse_Pending;
	}

	int CHttpParser::ReadHeaderValue(HttpBody *pMsg, CCircleQueue &queue)
	{
		int nLen = 0;
		while (GetNext(queue))
		{
			if (m_Current == '\r')
			{
				GetNext(queue);
				if (m_Current == '\n')
				{
					if (!strcmp(pMsg->arrHeaders[pMsg->nHeaderCount].szName, "Content-Length"))
					{
						m_nContentLen = atoi(pMsg->arrHeaders[pMsg->nHeaderCount].szValue);
					}
					pMsg->nHeaderCount++;
					m_nStatus = Status_Name;
					return Parse_Pending;
				}
				else
				{
					return Parse_Fail;
				}
			}
			else if (Is_Ctrl(m_Current))
			{
				return Parse_Fail;
			}
			else
			{
				pMsg->arrHeaders[pMsg->nHeaderCount].szValue[nLen++] = m_Current;
			}
		}
		return Parse_Pending;
	}

	int CHttpParser::ReadContent(HttpBody *pMsg, CCircleQueue &queue)
	{
		if (m_nContentLen > 0)
		{
			if (queue.GetSize() == m_nContentLen)
			{
				queue.Read(pMsg->szContent, m_nContentLen);
				return Parse_OK;
			}
			else if(queue.GetSize() < m_nContentLen)
			{
				return Parse_Pending;
			}
			else
			{
				return Parse_Fail;
			}
		}
		else
		{
			return Parse_OK;
		}
	}

	bool CHttpParser::Is_Char(int c)
	{
		return c >= 0 && c <= 127;
	}

	bool CHttpParser::Is_Ctrl(int c)
	{
		return (c >= 0 && c <= 31) || (c == 127);
	}

	bool CHttpParser::Is_Special(int c)
	{
		switch (c)
		{
		case '(': case ')': case '<': case '>': case '@':
		case ',': case ';': case ':': case '\\': case '"':
		case '/': case '[': case ']': case '?': case '=':
		case '{': case '}': case ' ': case '\t':
			return true;
		default:
			return false;
		}
	}

	bool CHttpParser::Is_Digit(int c)
	{
		return c >= '0' && c <= '9';
	}
}

