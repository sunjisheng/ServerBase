#include "CommonBase.h"
#include "Ini.h"

namespace Minicat
{
	CIni::CIni()
	{
		m_nFileLen = 0;
		m_szFileContent = NULL;
		IndexNum = 0;
		IndexList = NULL;
		memset(&m_szIniFileName,0,sizeof(m_szIniFileName));
		memset(&m_szValue,0,sizeof(m_szValue));
	}


	CIni::~CIni(void)
	{
		if(m_szFileContent != NULL)
		{
			free(m_szFileContent);
			m_szFileContent =NULL;
		}
		SAFE_DELETE_ARRAY(IndexList);
	}

	bool CIni::Open(const char *szIniFileName)
	{
		strncpy(m_szIniFileName, szIniFileName, Max_FullPath - 1);
		FILE *fstream = fopen(m_szIniFileName, "rb");
		if(fstream == NULL)
			return false;

		fseek( fstream, 0L, SEEK_END );
		m_nFileLen	= ftell( fstream );
		if(m_szFileContent != NULL)
		{
			free(m_szFileContent);
			m_szFileContent =NULL;
		}
		m_szFileContent = (char*)malloc(m_nFileLen + 1);
		m_szFileContent[m_nFileLen] = '\0';
		fseek(fstream, 0L,SEEK_SET);
		fread(m_szFileContent, m_nFileLen, 1, fstream);
		fclose(fstream);

		InitIndex();
		return true;
	}

	bool CIni::Reload()
	{
		if(strlen(m_szFileContent) == 0) return false;
		return Open(m_szIniFileName);
	}

	void CIni::InitIndex()
	{
		IndexNum=0;

		for(int i=0; i<m_nFileLen; i++)
		{
			//找到
			if( m_szFileContent[i]=='[' && (i==0 || m_szFileContent[i-1]=='\n') )
			{
				IndexNum++;
			}
		}

		//申请内存
		SAFE_DELETE_ARRAY( IndexList );
		if( IndexNum>0 )
			IndexList=new int[IndexNum];

		int n=0;

		for(int i=0; i<m_nFileLen; i++)
		{
			if( m_szFileContent[i]=='[' && (i == 0 || m_szFileContent[i-1]=='\n') )
			{
				IndexList[n]=i+1;
				n++;
			}
		}
	}

	int	CIni::FindIndex(char *string)
	{
		for(int i=0; i<IndexNum; i++)
		{
			char *str=ReadStr( IndexList[i] );
			if( strcmp(string, str) == 0 )
			{
				return IndexList[i];
			}
		}
		return -1;
	}

	int	CIni::FindData(int index, char *string)
	{
		int p=index;	//指针

		while(1)
		{
			p=GotoNextLine(p);
			char *name=ReadDataName(p);
			if( strcmp(string, name)==0 )
			{
				SAFE_DELETE( name );
				return p;
			}

			if ( name[0] == '[' )
			{
				SAFE_DELETE( name );
				return -1;
			}

			SAFE_DELETE( name );
			if( p>=m_nFileLen ) return -1;
		}
		return -1;
	}

	int	CIni::GotoNextLine(int p)
	{
		int i;
		for(i=p; i<m_nFileLen; i++)
		{
			if( m_szFileContent[i]=='\n' )
				return i+1;
		}
		return i;
	}

	char* CIni::ReadDataName(int &p)
	{
		char chr;
		char *Ret;
		int m=0;

		Ret=new char[64];
		memset(Ret, 0, 64);

		for(int i=p; i<m_nFileLen; i++)
		{
			chr = m_szFileContent[i];

			//结束
			if( chr == '\r')
			{
				p=i+1;
				return Ret;
			}

			if( chr == '\n')
			{
				return Ret;
			}

			//结束
			if( chr == '=' || chr == ';' )
			{
				p=i+1;
				return Ret;
			}

			Ret[m]=chr;
			m++;
		}
		return Ret;
	}

	char *CIni::ReadStr(int p)
	{
		char chr;
		char *Ret;
		int n=p, m=0;

		int LineNum = GotoNextLine(p) - p + 1;
		Ret=(char*)m_szValue;
		memset(Ret, 0, LineNum);

		for(int i=0; i<m_nFileLen-p; i++)
		{
			chr = m_szFileContent[n];

			//结束
			if( chr == ';' || chr == '\r' || chr == '\n' || chr == '\t' || chr == ']' )
			{
				return Ret;
			}

			Ret[m]=chr;
			m++;
			n++;
		}

		return Ret;
	}

	int	CIni::ReadInt(const char *szSection, const char *szKey, int nDefault)
	{
		int n = FindIndex((char*)szSection);
		if(n == -1)
			return nDefault;
		int m = FindData(n, (char*)szKey);
		if(m == -1)
			return nDefault;
		char *str = ReadStr(m);
		return atoi(str);
	}

	char* CIni::ReadStr(const char *szSection, const char *szKey, char* szStr, int nSize)
	{
		int n = FindIndex((char*)szSection);
		if ( n == -1 )
			return NULL;
		int m = FindData(n, (char*)szKey);
		if ( m == -1 )
			return NULL;

		char* szText = ReadStr(m);
		strncpy( szStr, szText, nSize ) ;
		return szText ;
	}
}



