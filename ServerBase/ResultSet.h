#pragma once
#include <mysql.h>
namespace Minicat
{
	class CResultSet
	{
	public:
		CResultSet()
		{
			m_pResult = NULL;
			m_pCurRow = NULL;
			m_pColLens = NULL;
		}

		~CResultSet()
		{
			if (m_pResult != NULL)
			{
				mysql_free_result(m_pResult);
				m_pResult = NULL;
			}
		}

		inline int GetRowCount()
		{
			if (m_pResult == NULL)
			{
				return 0;
			}
			return (int)mysql_num_rows(m_pResult);
		}

		inline int GetColCount()
		{
			if (m_pResult == NULL)
			{
				return 0;
			}
			return (int)mysql_num_fields(m_pResult);
		}

		inline bool FetchRow()
		{
			if (m_pResult == NULL)
			{
				return false;
			}
			m_pCurRow = mysql_fetch_row(m_pResult);
			if (!m_pCurRow)
			{
				return false;
			}
			m_pColLens = mysql_fetch_lengths(m_pResult);
			if (!m_pColLens)
			{
				return false;
			}
			return true;
		}

		inline bool CanGetValue(int nCol)
		{
			if (m_pResult == NULL)
			{
				return false;
			}

			if (nCol >= GetColCount())
			{
				return false;
			}

			if (m_pCurRow == NULL)
			{
				return false;
			}

			if (m_pColLens[nCol] == 0)
			{
				return false;
			}
			return true;
		}

		bool GetIntValue(int nCol, int& nValue)
		{
			if (!CanGetValue(nCol))
			{
				return false;
			}
			nValue = atoi(m_pCurRow[nCol]);
			return true;
		}

		bool GetInt64Value(int nCol, long long& lValue)
		{
			if (!CanGetValue(nCol))
			{
				return false;
			}
			lValue = atoll(m_pCurRow[nCol]);
			return true;
		}

		bool GetString(int nCol, char* szValue, int nLen)
		{
			if (!CanGetValue(nCol))
			{
				return false;
			}
			int nColLen = m_pColLens[nCol];
			if (nLen < nColLen + 1)
			{
				return false;
			}

			memcpy(szValue, m_pCurRow[nCol], nColLen);
			szValue[nColLen] = 0;
			return true;
		}

		bool GetBinary(int nCol, char* szValue, int nLen, int &nOutLen)
		{
			if (!CanGetValue(nCol))
			{
				return false;
			}
			nOutLen = m_pColLens[nCol];
			if (nLen < nOutLen)
			{
				return false;
			}
			memcpy(szValue, m_pCurRow[nCol], nOutLen);
			return true;
		}
	public:
		MYSQL_RES *m_pResult;
		MYSQL_ROW m_pCurRow;
		unsigned long *m_pColLens;
	};
}