// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

#ifndef PCH_H
#define PCH_H

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
#define _WIN32_WINNT 0x0501
#endif
#include <WinSock2.h>  //必须在Windows.h前定义
#include <MSWSock.h>
#include <Windows.h>
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
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
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
#include <mutex>
#include <condition_variable>
#include <deque>
#include <vector>
#include <algorithm>
#include <limits>
#include <thread>
#include <chrono>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;


// TODO: 添加要在此处预编译的标头
#include "List.h"
#include "Vector.h"
#include "HashMap.h"
#include "CommonBase.h"
using namespace Minicat;
#endif //PCH_H
