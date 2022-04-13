/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  通用定义文件
**********************************************************************************/
#pragma once

namespace Minicat
{
	//根据指针调用free接口
#ifndef SAFE_DELETE
#define SAFE_DELETE(x)	if( (x)!=nullptr ) { delete (x); (x)=nullptr; }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x)	if( (x)!=nullptr ) { delete[] (x); (x)=nullptr; }
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(x)	if( (x)!=nullptr ) { free(x); (x)=nullptr; }
#endif

#ifndef NULL
#define NULL 0
#endif

#if defined(_WINDOWS)
#endif

#if defined(_LINUX)
#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif
#endif

	enum Const_Max_Def
	{
		Max_FullPath = 255,
		Max_Logger_Name = 32,
		Max_UInt = 4294967295,
	}; 
}