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
		void InitIndex();						//��ʼ������
		int	FindIndex(char *);					//���ر���λ��
		int	FindData(int, char *);				//��������λ��
		int			GotoNextLine(int); 			//����
		char*		ReadDataName(int &);		//��ָ��λ�ö�һ��������

		char m_szIniFileName[Max_FullPath];
		int	m_nFileLen;					//�ļ�����
		char* m_szFileContent;			//�ļ�����

		int	IndexNum;					//������Ŀ��[]����Ŀ��
		int* IndexList;					//������λ���б�

		char m_szValue[512];
	};
}
