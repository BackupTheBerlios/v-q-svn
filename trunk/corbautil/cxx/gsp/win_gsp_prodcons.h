//-----------------------------------------------------------------------
// Copyright IONA Technologies 2002-2004. All rights reserved.
// This software is provided "as is".
//
// File:	win_gsp_prodcons.h
//
// Policy:	ProdCons[PutOp, GetOp, OtherOp]
//
// Description:	The producer-consumer synchronisation policy.
//----------------------------------------------------------------------


#ifndef WIN_GSP_PRODCONS_H_
#define WIN_GSP_PRODCONS_H_





//--------
// #include's
//--------
#include <windows.h>
#include <process.h>
#include <limits.h>
#include <assert.h>





//--------
// Forward declarations.
//--------
class GSP_ProdCons;





class GSP_ProdCons {
public:
	inline GSP_ProdCons();
	inline ~GSP_ProdCons();


	class PutOp {
	public:
		inline PutOp(GSP_ProdCons &);
		inline ~PutOp();

	protected:
		GSP_ProdCons	&m_sync;
	};

	class GetOp {
	public:
		inline GetOp(GSP_ProdCons &);
		inline ~GetOp();

	protected:
		GSP_ProdCons	&m_sync;
	};

	class OtherOp {
	public:
		inline OtherOp(GSP_ProdCons &);
		inline ~OtherOp();

	protected:
		GSP_ProdCons	&m_sync;
	};

protected:
	friend	class ::GSP_ProdCons::PutOp;
	friend	class ::GSP_ProdCons::GetOp;
	friend	class ::GSP_ProdCons::OtherOp;

	HANDLE	m_mutex;	// mutex
	HANDLE	m_item_count;	// semaphore; counts number of items in buffer
};





//--------
// Inline implementation of class GSP_ProdCons
//--------

inline GSP_ProdCons::GSP_ProdCons()
{
	SECURITY_ATTRIBUTES	attr;

	attr.nLength              = sizeof(SECURITY_ATTRIBUTES);
	attr.lpSecurityDescriptor = NULL;
	attr.bInheritHandle       = TRUE;

	m_mutex = CreateMutex(&attr, FALSE, 0);
	assert (m_mutex != 0);

	m_item_count = CreateSemaphore(&attr, 0, LONG_MAX, 0);
	assert (m_item_count != 0);
}


inline GSP_ProdCons::~GSP_ProdCons()
{
	BOOL	status;

	status = CloseHandle(m_mutex);
	assert(status == TRUE);

	status = CloseHandle(m_item_count);
	assert(status == TRUE);
}





//--------
// Inline implementation of class GSP_ProdCons::PutOp
//--------

inline GSP_ProdCons::PutOp::PutOp(GSP_ProdCons &sync_data)
	: m_sync(sync_data)
{
	DWORD	status;

	status = WaitForSingleObject(m_sync.m_mutex, INFINITE);
	assert(status == WAIT_OBJECT_0);
}


inline GSP_ProdCons::PutOp::~PutOp()
{
	BOOL	status;

	status = ReleaseMutex(m_sync.m_mutex);
	assert(status == TRUE);

	status = ReleaseSemaphore(m_sync.m_item_count, 1, 0);
	assert(status == TRUE);
}





//--------
// Inline implementation of class GSP_ProdCons::GetOp
//--------

inline GSP_ProdCons::GetOp::GetOp(GSP_ProdCons &sync_data)
	: m_sync(sync_data)
{
	DWORD	status;

	status = WaitForSingleObject(m_sync.m_item_count, INFINITE);
	assert(status == WAIT_OBJECT_0);

	status = WaitForSingleObject(m_sync.m_mutex, INFINITE);
	assert(status == WAIT_OBJECT_0);
}


inline GSP_ProdCons::GetOp::~GetOp()
{
	BOOL	status;

	status = ReleaseMutex(m_sync.m_mutex);
	assert(status == TRUE);
}





//--------
// Inline implementation of class GSP_ProdCons::OtherOp
//--------

inline GSP_ProdCons::OtherOp::OtherOp(GSP_ProdCons &sync_data)
	: m_sync(sync_data)
{
	DWORD	status;

	status = WaitForSingleObject(m_sync.m_mutex, INFINITE);
	assert(status == WAIT_OBJECT_0);
}


inline GSP_ProdCons::OtherOp::~OtherOp()
{
	BOOL	status;

	status = ReleaseMutex(m_sync.m_mutex);
	assert(status == TRUE);
}





#endif
