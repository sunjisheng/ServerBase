/*********************************************************************************
  *����:  SunJiSheng
  *����:  2020/04/15
  *����:  ͨ�ú���
**********************************************************************************/

#pragma once
#include "CommonBase.h"

namespace Minicat
{
	//��ȡ��ǰ��������Tick
	uint64 GetTickCount();
	//��ȡ�ļ�·��
	int GetFilePath(const char *szFileName, char *szPath, int nLen);
	//��ȡ��ǰ·��
	int GetCurrentPath(char *szPath, int nLen);
	//��ȡ�ļ���С
	int MyGetFileSize(char*szFileName);
	//linux�ػ�����
	void Daemonize();
	//��ȡ��ǰ�߳�ID
	string GetCurrentThreadID();
	//����
	void MySleep(unsigned int nMS);
	//����IP:Port
	bool ParseIPPort(const char *str, char *szIP, int& nPort);
	//�ַ�����ϣ
	uint64 Hash(const char *str);
	//����64λΨһID
	int64 GenUniqueID(int nServerID, int nObjectType);
	//�Ƿ���IP
	bool IsIPAddr(const char *szIP);
    //����תIP
	void ConvertDomain2IP(char *szDomain, char *szszIP);
	//URL����
	void ParseURL(char *szUrl, char *szHost, int HostLen, char *szDir, int nDirLen);
	//16�����ַ���ת��
	void Hex2Str(char *szOut, const char *szInput, int nLen);
	void Str2Hex(char *szOut, const char *szInput, int nLen);
	//Des���ܽ���
	int Encrypt(char *szOut, const char *szInput, int nLen, const char *szKey, int nKeyLen);
	int Decrypt(char *szOut, const char *szInput, int nLen, const char *szKey, int nKeyLen);
	//MD5
	void GetMD5(char szMD5[32], const char *szSrc);
	//Split�ַ���
	void SplitStr(string &str, char sep, vector<string> &ve);
	//�Ƿ�Utf8����
	int IsUtf8(const char *str, int len);
};