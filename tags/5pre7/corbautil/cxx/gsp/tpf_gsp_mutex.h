#ifdef __370__
#pragma nomargins
#endif

//-----------------------------------------------------------------------
// Copyright IONA Technologies 2002-2004. All rights reserved.
// This software is provided "as is".
//
// File:	tpf_gsp_mutex.h
//
// Policy:	GSP_Mutex[Op]	// non-recursive mutex
//----------------------------------------------------------------------

#ifndef TPF_GSP_MUTEX_H_
#define TPF_GSP_MUTEX_H_
 
 



//--------
// #include's
//--------
#include <tpfapi.h>
#include <assert.h>





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
	double		m_lock;
};





//--------
// Inline implementation of class GSP_Mutex
//--------

inline GSP_Mutex::GSP_Mutex()
{
	//--------
	// Nothing to do
	//--------
}



inline GSP_Mutex::~GSP_Mutex()
{
	//--------
	// Nothing to do
	//--------
}





//--------
// Inline implementation of class GSP_Mutex::Op
//--------

inline GSP_Mutex::Op::Op(GSP_Mutex &sync_data)
        : m_sync(sync_data)
{
	int	status;

	status = lockc((void *)&m_sync.m_lock, LOCK_O_SPIN);
	assert(status == 0);
}



inline GSP_Mutex::Op::~Op()
{
	int	status;

	status = unlkc((void *)&m_sync.m_lock);
	assert(status == 0);
}

#endif
