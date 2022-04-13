/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  通用包含文件
**********************************************************************************/
#pragma once

#ifndef _WINDOWS
#define _LINUX
#endif

#if defined(_WINDOWS)
#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4003 )
#pragma warning ( disable : 4291 )

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <WinSock2.h>  //必须在Windows.h前定义
#include <MSWSock.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <Winsvc.h>
#include <WinBase.h>
#include <WinDef.h>
#include <process.h>
#include <Mmsystem.h>
#include <atomic>
#include <TCHAR.H>
#include <io.h>
#elif defined(_LINUX)
#include <sys/types.h>
#include <pthread.h>
#include <execinfo.h>
#include <signal.h>
#include <exception>
#include <setjmp.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#endif

//标准库
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <sstream>
#include <locale.h>
#include <list>
#include <map>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <deque>
#include <vector>
#include <algorithm>
#include <limits>
#include <thread>
#include <chrono>
#include <bitset>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;
//第三方
#define USE_JEMALLOC 1
#define JEMALLOC_MANGLE
#include <jemalloc.h>
//通用
#include "CommonDefine.h"
#include "CommonType.h"