#pragma once

#include <Windows.h>

class Lock
{
public:
	Lock()
	{
		InitializeCriticalSection(&m_cs);
	}

	~Lock()
	{
		DeleteCriticalSection(&m_cs);
	}

	void Enter()
	{
		EnterCriticalSection(&m_cs);
	}

	void Leave()
	{
		LeaveCriticalSection(&m_cs);
	}

private:
	CRITICAL_SECTION m_cs;
};

class LockGuard
{
public:
	LockGuard(Lock& lock) : _lock(lock)
	{
		_lock.Enter();
	}

	~LockGuard()
	{
		_lock.Leave();
	}
private:
	Lock& _lock;
};