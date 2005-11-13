//-----------------------------------------------------------------------
// Copyright IONA Technologies 2002-2004. All rights reserved.
// This software is provided "as is".
//
// File:	dce_gsp_mutex.h
//
// Policy:	GSP_Mutex[Op]	// non-recursive mutex
//----------------------------------------------------------------------

#ifndef DCE_GSP_MUTEX_H_
#define DCE_GSP_MUTEX_H_
 
 



//--------
// #include's
//--------
#include <pthread.h>
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
		GSP_Mutex	&m_sync;
	};
 
protected:
	friend class	Op;
	pthread_mutex_t m_mutex;
};





//--------
// Inline implementation of class GSP_Mutex
//--------

inline GSP_Mutex::GSP_Mutex()
{
	int status;

	status = pthread_mutex_init(&m_mutex, pthread_mutexattr_default);
	assert(status == 0);
}


inline GSP_Mutex::~GSP_Mutex()
{
	int status;

	status = pthread_mutex_destroy(&m_mutex);
	assert(status == 0);
}





//--------
// Inline implementation of class GSP_Mutex::Op
//--------

inline GSP_Mutex::Op::Op(GSP_Mutex &sync_data)
        : m_sync(sync_data)
{
	int status;

	status = pthread_mutex_lock(&m_sync.m_mutex);
	assert(status == 0);
}


inline GSP_Mutex::Op::~Op()
{
	int status;

	status = pthread_mutex_unlock(&m_sync.m_mutex);
	assert(status == 0);
}





#endif
