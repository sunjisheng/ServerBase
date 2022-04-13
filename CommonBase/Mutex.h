#pragma once
#include "CommonBase.h"
//»¥³âËø
#if defined(_WINDOWS)
class CMutex
{
	CRITICAL_SECTION m_Lock;
public:
	CMutex() { InitializeCriticalSection(&m_Lock); };
	~CMutex() { DeleteCriticalSection(&m_Lock); };
	void	Lock() { EnterCriticalSection(&m_Lock); };
	void	Unlock() { LeaveCriticalSection(&m_Lock); };
};
#elif defined(_LINUX)
class CMutex
{
	pthread_mutex_t 	m_Mutex;
public:
	CMutex()
	{
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
		pthread_mutex_init(&m_Mutex, &attr);
	};
	~CMutex() { pthread_mutex_destroy(&m_Mutex); };
	void	Lock() { pthread_mutex_lock(&m_Mutex); };
	void	Unlock() { pthread_mutex_unlock(&m_Mutex); };
};
#endif

class CAutoLock
{
public:
	CAutoLock(CMutex& lock) :m_lock(lock)
	{
		m_lock.Lock();
	}
	CAutoLock(CMutex& lock, const char *szFile, int nLine) :m_lock(lock)
	{
		m_lock.Lock();
	}
	~CAutoLock()
	{
		m_lock.Unlock();
	}
	CMutex &m_lock;
};
