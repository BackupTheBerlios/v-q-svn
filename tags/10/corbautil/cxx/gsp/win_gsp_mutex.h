//-----------------------------------------------------------------------
// Copyright IONA Technologies 2002-2004. All rights reserved.
// This software is provided "as is".
//
// File:	win_gsp_mutex.h
//
// Policy:	Mutex[Op]	// non-recurisve mutex
//----------------------------------------------------------------------

#ifndef WIN_GSP_MUTEX_H_
#define WIN_GSP_MUTEX_H_





//--------
// #include's
//--------
#include <windows.h>
#include <process.h>
#include <assert.h>





//--------
// Forward declarations.
//--------
class GSP_Mutex;





class GSP_Mutex {
public:
	inline GSP_Mutex();
	inline ~GSP_Mutex();

	class Op {
	public:
		inline Op(GSP_Mutex &);
		inline ~Op();

	protected:
		GSP_Mutex      &m_sync;
	};

protected:
	friend  class ::GSP_Mutex::Op;
	HANDLE	m_mutex;
	int	m_deadlockIfReacquired;
};





//--------
// Inline implementation of class GSP_Mutex
//--------

inline GSP_Mutex::GSP_Mutex()
{
	SECURITY_ATTRIBUTES	attr;

	attr.nLength              = sizeof(SECURITY_ATTRIBUTES);
	attr.lpSecurityDescriptor = NULL;
	attr.bInheritHandle       = TRUE;

	m_mutex = CreateMutex(&attr, FALSE, 0);
	m_deadlockIfReacquired = 0;
}



inline GSP_Mutex::~GSP_Mutex()
{
	CloseHandle(m_mutex);
}





//--------
// Inline implementation of class GSP_Mutex::Op
//--------

inline GSP_Mutex::Op::Op(GSP_Mutex &sync_data)
        : m_sync(sync_data)
{
	WaitForSingleObject(m_sync.m_mutex, INFINITE);
	assert(m_sync.m_deadlockIfReacquired == 0);
	m_sync.m_deadlockIfReacquired = 1;
}



inline GSP_Mutex::Op::~Op()
{
	assert(m_sync.m_deadlockIfReacquired);
	m_sync.m_deadlockIfReacquired = 0;
	ReleaseMutex(m_sync.m_mutex);
}





#endif
