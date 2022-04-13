/*********************************************************************************
  *����:  SunJiSheng
  *����:  2020/04/15
  *����:  ��дȫ��new delete opeerator, ��Jemalloc�ӹ��ڴ����
**********************************************************************************/

#pragma once
#include "CommonBase.h"

#ifdef USE_JEMALLOC
void* operator new(size_t nBytes);
void* operator new[](size_t nBytes);
void operator delete(void *ptr) noexcept;
void operator delete[](void *ptr) noexcept;
#endif // 