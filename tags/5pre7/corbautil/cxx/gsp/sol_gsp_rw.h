//-----------------------------------------------------------------------
// Copyright IONA Technologies 2002-2004. All rights reserved.
// This software is provided "as is".
//
// File:	sol_gsp_rw.h
//
// Policy:	RW[ReadOp, WriteOp]	// readers-writer lock
//----------------------------------------------------------------------

#ifndef SOL_GSP_RW_H_
#define SOL_GSP_RW_H_





//--------
// #include's
//--------
#include <pthread.h>
#include <assert.h>





//--------
// Forward declarations.
//--------
class GSP_RW;





class GSP_RW {
public:
	inline GSP_RW();
	inline ~GSP_RW();

	class ReadOp {
	public:
		inline ReadOp(GSP_RW &);
		inline ~ReadOp();

	protected:
		GSP_RW      &m_sync;
	};

	class WriteOp {
	public:
		inline WriteOp(GSP_RW &);
		inline ~WriteOp();

	protected:
		GSP_RW      &m_sync;
	};

protected:
	friend  class ::GSP_RW::ReadOp;
	friend  class ::GSP_RW::WriteOp;
	rwlock_t	m_lock;
};





//--------
// Inline implementation of class GSP_RW
//--------

inline GSP_RW::GSP_RW()
{
	int	status;

	status = rwlock_init(&m_lock, USYNC_THREAD, 0);
	assert(status == 0);
}



inline GSP_RW::~GSP_RW()
{
	int	status;

	status = rwlock_destroy(&m_lock);
	assert(status == 0);
}





//--------
// Inline implementation of class GSP_RW::ReadOp
//--------

inline GSP_RW::ReadOp::ReadOp(GSP_RW &sync_data)
        : m_sync(sync_data)
{
	int	status;

	status = rw_rdlock(&m_sync.m_lock);
	assert(status == 0);
}



inline GSP_RW::ReadOp::~ReadOp()
{
	int	status;

	status = rw_unlock(&m_sync.m_lock);
	assert(status == 0);
}





//--------
// Inline implementation of class GSP_RW::WriteOp
//--------

inline GSP_RW::WriteOp::WriteOp(GSP_RW &sync_data)
        : m_sync(sync_data)
{
	int	status;

	status = rw_wrlock(&m_sync.m_lock);
	assert(status == 0);
}



inline GSP_RW::WriteOp::~WriteOp()
{
	int	status;

	status = rw_unlock(&m_sync.m_lock);
	assert(status == 0);
}





#endif
