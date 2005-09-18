//-----------------------------------------------------------------------
// Copyright IONA Technologies 2002-2004. All rights reserved.
// This software is provided "as is".
//
// File:	sol_gsp_mutex.h
//
// Policy:	GSP_Mutex[Op]	// non-recursive mutex
//----------------------------------------------------------------------

#ifndef SOL_GSP_MUTEX_H_
#define SOL_GSP_MUTEX_H_
 
 



//--------
// #include's
//--------
#include <synch.h>
#include <thread.h>





//--------
// Forward declarations
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
		GSP_Mutex	&m_sync;
	};
 
protected:
	friend class	Op;
	mutex_t		m_mutex;
};





//--------
// Inline implementation of class GSP_Mutex
//--------

inline GSP_Mutex::GSP_Mutex()
{
	int status;

	status = mutex_init(&m_mutex, USYNC_THREAD, 0);
	assert(status == 0);
}



inline GSP_Mutex::~GSP_Mutex()
{
	int status;

	status = mutex_destroy(&m_mutex);
	assert(status == 0);
}





//--------
// Inline implementation of class GSP_Mutex::Op
//--------

inline GSP_Mutex::Op::Op(GSP_Mutex &sync_data)
        : m_sync(sync_data)
{
	int status;

	status = mutex_lock(&m_sync.m_mutex);
	assert(status == 0);
}



inline GSP_Mutex::Op::~Op()
{
	int status;

	status = mutex_unlock(&m_sync.m_mutex);
	assert(status == 0);
}

#endif
