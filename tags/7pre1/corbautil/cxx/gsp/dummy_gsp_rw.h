#ifdef __370__
#pragma nomargins
#endif

//-----------------------------------------------------------------------
// Copyright IONA Technologies 2002-2004. All rights reserved.
// This software is provided "as is".
//
// File:	dummy_gsp_rw.h
//
// Policy:	RW[ReadOp, WriteOp]	// readers-writer lock
//----------------------------------------------------------------------

#ifndef DUMMY_GSP_RW_H_
#define DUMMY_GSP_RW_H_





//--------
// #include's
//--------
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

	int	m_reader_count;
	int	m_writer_count;
};





//--------
// Inline implementation of class GSP_RW
//--------

inline GSP_RW::GSP_RW()
{
	m_reader_count = 0;
	m_writer_count = 0;
}



inline GSP_RW::~GSP_RW()
{
}





//--------
// Inline implementation of class GSP_RW::ReadOp
//--------

inline GSP_RW::ReadOp::ReadOp(GSP_RW &sync_data)
        : m_sync(sync_data)
{
	assert(m_sync.m_writer_count == 0);
	m_sync.m_reader_count++;
}



inline GSP_RW::ReadOp::~ReadOp()
{
	assert(m_sync.m_writer_count == 0);
	m_sync.m_reader_count--;
}





//--------
// Inline implementation of class GSP_RW::WriteOp
//--------

inline GSP_RW::WriteOp::WriteOp(GSP_RW &sync_data)
        : m_sync(sync_data)
{
	assert(m_sync.m_writer_count == 0);
	assert(m_sync.m_reader_count == 0);
	m_sync.m_writer_count++;
}



inline GSP_RW::WriteOp::~WriteOp()
{
	assert(m_sync.m_writer_count == 1);
	assert(m_sync.m_reader_count == 0);
	m_sync.m_writer_count--;
}





#endif
