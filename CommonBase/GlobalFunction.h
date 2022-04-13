/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  通用函数
**********************************************************************************/

#pragma once
#include "CommonBase.h"

namespace Minicat
{
	//获取当前程序运行Tick
	uint64 GetTickCount();
	//获取文件路径
	int GetFilePath(const char *szFileName, char *szPath, int nLen);
	//获取当前路径
	int GetCurrentPath(char *szPath, int nLen);
	//获取文件大小
	int MyGetFileSize(char*szFileName);
	//linux守护进程
	void Daemonize();
	//获取当前线程ID
	string GetCurrentThreadID();
	//休眠
	void MySleep(unsigned int nMS);
	//解析IP:Port
	bool ParseIPPort(const char *str, char *szIP, int& nPort);
	//字符串哈希
	uint64 Hash(const char *str);
	//生成64位唯一ID
	int64 GenUniqueID(int nServerID, int nObjectType);
	//是否是IP
	bool IsIPAddr(const char *szIP);
    //域名转IP
	void ConvertDomain2IP(char *szDomain, char *szszIP);
	//URL解析
	void ParseURL(char *szUrl, char *szHost, int HostLen, char *szDir, int nDirLen);
	//16进制字符串转换
	void Hex2Str(char *szOut, const char *szInput, int nLen);
	void Str2Hex(char *szOut, const char *szInput, int nLen);
	//Des加密解密
	int Encrypt(char *szOut, const char *szInput, int nLen, const char *szKey, int nKeyLen);
	int Decrypt(char *szOut, const char *szInput, int nLen, const char *szKey, int nKeyLen);
	//MD5
	void GetMD5(char szMD5[32], const char *szSrc);
	//Split字符串
	void SplitStr(string &str, char sep, vector<string> &ve);
	//是否Utf8编码
	int IsUtf8(const char *str, int len);
};