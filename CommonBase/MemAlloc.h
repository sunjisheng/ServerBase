/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  重写全局new delete opeerator, 用Jemalloc接管内存分配
**********************************************************************************/

#pragma once
#include "CommonBase.h"

#ifdef USE_JEMALLOC
void* operator new(size_t nBytes);
void* operator new[](size_t nBytes);
void operator delete(void *ptr) noexcept;
void operator delete[](void *ptr) noexcept;
#endif // 