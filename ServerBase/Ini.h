#pragma once
#include "CommonDefine.h"
namespace Minicat
{
	class CIni
	{
	public:
		CIni();
		~CIni(void);
	public:
		bool Open(const char *szIniFileName);
		const char *GetFileName() { return &m_szIniFileName[0]; }
		bool Reload();
		int	ReadInt(const char *szSection, const char *szKey, int nDefault);
		char* ReadStr(const char *szSection, const char *szKey, char* szStr, int nSize);
	protected:

		char* ReadStr(int p);
		void InitIndex();						//初始化索引
		int	FindIndex(char *);					//返回标题位置
		int	FindData(int, char *);				//返回数据位置
		int			GotoNextLine(int); 			//提行
		char*		ReadDataName(int &);		//在指定位置读一数据名称

		char m_szIniFileName[Max_FullPath];
		int	m_nFileLen;					//文件长度
		char* m_szFileContent;			//文件内容

		int	IndexNum;					//索引数目（[]的数目）
		int* IndexList;					//索引点位置列表

		char m_szValue[512];
	};
}
