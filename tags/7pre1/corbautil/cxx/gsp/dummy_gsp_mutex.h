#ifdef __370__
#pragma nomargins
#endif

//-----------------------------------------------------------------------
// Copyright IONA Technologies 2002-2004. All rights reserved.
// This software is provided "as is".
//
// File:	dummy_gsp_mutex.h
//
// Policy:	GSP_Mutex[Op]	// non-recursive mutex
//----------------------------------------------------------------------

#ifndef DUMMY_GSP_MUTEX_H_
#define DUMMY_GSP_MUTEX_H_
 
 



//--------
// #include's
//--------
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

	int	m_in_critical_section; // Boolean
};





//--------
// Inline implementation of class GSP_Mutex
//--------

inline GSP_Mutex::GSP_Mutex()
{
	m_in_critical_section = 0;
}


inline GSP_Mutex::~GSP_Mutex()
{
}





//--------
// Inline implementation of class GSP_Mutex::Op
//--------

inline GSP_Mutex::Op::Op(GSP_Mutex &sync_data)
        : m_sync(sync_data)
{
	assert(!m_sync.m_in_critical_section);
	m_sync.m_in_critical_section = 1;
}


inline GSP_Mutex::Op::~Op()
{
	assert(m_sync.m_in_critical_section);
	m_sync.m_in_critical_section = 0;
}





#endif
