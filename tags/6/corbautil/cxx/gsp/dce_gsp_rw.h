//-----------------------------------------------------------------------
// Copyright IONA Technologies 2002-2004. All rights reserved.
// This software is provided "as is".
//
// File:	dce_gsp_rw.h
//
// Policy:	RW[ReadOp, WriteOp]	// readers-writer lock
//
// Note:	The algorithm is taken from "Programming with POSIX
//		Threads" by David R. Butenhof and adapted for DCE
//		threads.
//----------------------------------------------------------------------

#ifndef DCE_GSP_RW_H_
#define DCE_GSP_RW_H_





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
	pthread_mutex_t	m_mutex;
	pthread_cond_t	m_read_cond;
	pthread_cond_t	m_write_cond;
	int		m_reader_count;
	int		m_writer_count;		// really a boolean
	int		m_reader_waiting_count;
	int		m_writer_waiting_count;
};





//--------
// Inline implementation of class GSP_RW
//--------

inline GSP_RW::GSP_RW()
{
	int	status;

	m_reader_count         = 0;
	m_writer_count         = 0;
	m_reader_waiting_count = 0;
	m_writer_waiting_count = 0;

	status = pthread_mutex_init(&m_mutex, pthread_mutexattr_default);
	assert(status == 0);

	status = pthread_cond_init(&m_read_cond, pthread_condattr_default);
	assert(status == 0);

	status = pthread_cond_init(&m_write_cond, pthread_condattr_default);
	assert(status == 0);
}



inline GSP_RW::~GSP_RW()
{
	int	status;

	//--------
	// Sanity checks
	//--------
	status = pthread_mutex_lock(&m_mutex);
	assert(status == 0);
		assert(m_reader_count == 0);
		assert(m_writer_count == 0);
		assert(m_reader_waiting_count == 0);
		assert(m_writer_waiting_count == 0);
	status = pthread_mutex_unlock(&m_mutex);
	assert(status == 0);

	//--------
	// Now destroy everything
	//--------
	status = pthread_mutex_destroy(&m_mutex);
	assert(status == 0);

	status = pthread_cond_destroy(&m_read_cond);
	assert(status == 0);

	status = pthread_cond_destroy(&m_write_cond);
	assert(status == 0);
}





//--------
// Inline implementation of class GSP_RW::ReadOp
//--------

inline GSP_RW::ReadOp::ReadOp(GSP_RW &sync_data)
        : m_sync(sync_data)
{
	int	status;

	status = pthread_mutex_lock(&m_sync.m_mutex);
	assert(status == 0);

	if (m_sync.m_writer_count) {
		m_sync.m_reader_waiting_count ++;

		while (m_sync.m_writer_count) {
			status = pthread_cond_wait(&m_sync.m_read_cond,
						   &m_sync.m_mutex);
			assert(status == 0);
		}

		m_sync.m_reader_waiting_count --;
	}

	m_sync.m_reader_count ++;

	status = pthread_mutex_unlock(&m_sync.m_mutex);
	assert(status == 0);
}



inline GSP_RW::ReadOp::~ReadOp()
{
	int	status;

	status = pthread_mutex_lock(&m_sync.m_mutex);
	assert(status == 0);

	m_sync.m_reader_count --;

	if (m_sync.m_reader_count == 0 && m_sync.m_writer_waiting_count > 0) {
			status = pthread_cond_signal(&m_sync.m_write_cond);
			assert(status == 0);
	}

	status = pthread_mutex_unlock(&m_sync.m_mutex);
	assert(status == 0);
}





//--------
// Inline implementation of class GSP_RW::WriteOp
//--------

inline GSP_RW::WriteOp::WriteOp(GSP_RW &sync_data)
        : m_sync(sync_data)
{
	int	status;

	status = pthread_mutex_lock(&m_sync.m_mutex);
	assert(status == 0);

	if (m_sync.m_writer_count || m_sync.m_reader_count > 0) {
		m_sync.m_writer_waiting_count ++;
		while (m_sync.m_writer_count || m_sync.m_reader_count > 0) {
			status = pthread_cond_wait(&m_sync.m_write_cond,
						   &m_sync.m_mutex);
			assert(status == 0);
		}
		m_sync.m_writer_waiting_count --;
	}

	assert(m_sync.m_writer_count == 0);
	m_sync.m_writer_count = 1;

	status = pthread_mutex_unlock(&m_sync.m_mutex);
	assert(status == 0);
}



inline GSP_RW::WriteOp::~WriteOp()
{
	int	status;

	status = pthread_mutex_lock(&m_sync.m_mutex);
	assert(status == 0);

	assert(m_sync.m_writer_count == 1);
	m_sync.m_writer_count = 0;

	if (m_sync.m_reader_waiting_count > 0) {
		status = pthread_cond_broadcast(&m_sync.m_read_cond);
		assert(status == 0);
	} else if (m_sync.m_writer_waiting_count > 0) {
		status = pthread_cond_signal(&m_sync.m_write_cond);
		assert(status == 0);
	}

	status = pthread_mutex_unlock(&m_sync.m_mutex);
	assert(status == 0);
}





#endif
