#include "MemAlloc.h"
#ifdef USE_JEMALLOC
void* operator new(size_t nBytes)
{
	return malloc(nBytes);
}

void* operator new[](size_t nBytes)
{
	return operator new(nBytes);
}

void operator delete(void *ptr) noexcept
{
	free(ptr);
}

void operator delete[](void *ptr) noexcept
{
	operator delete(ptr);
}
#endif