//-----------------------------------------------------------------------
// Copyright IONA Technologies 2002-2004. All rights reserved.
// This software is provided "as is".
//
// File:	sol_gsp_prodcons.h
//
// Policy:	ProdCons[PutOp, GetOp, OtherOp]
//
// Description:	The producer-consumer synchronisation policy.
//----------------------------------------------------------------------


#ifndef SOL_GSP_PRODCONS_H_
#define SOL_GSP_PRODCONS_H_





//--------
// #include's
//--------
#include <synch.h>
#include <assert.h>





//--------
// Forward declarations
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
	friend class	::GSP_ProdCons::PutOp;
	friend class	::GSP_ProdCons::GetOp;
	friend class	::GSP_ProdCons::OtherOp;
	mutex_t		m_mutex;
	sema_t		m_item_count;
};





//--------
// Inline implementation of class GSP_ProdCons
//--------

inline GSP_ProdCons::GSP_ProdCons()
{
	int	status;

	status = mutex_init(&m_mutex, USYNC_THREAD, 0);
	assert(status == 0);

	status = sema_init(&m_item_count, 0, USYNC_THREAD, 0);
	assert(status == 0);
}


inline GSP_ProdCons::~GSP_ProdCons()
{
	int	status;

	status = mutex_destroy(&m_mutex);
	assert(status == 0);

	status = sema_destroy(&m_item_count);
	assert(status == 0);
}





//--------
// Inline implementation of class GSP_ProdCons::PutOp
//--------

inline GSP_ProdCons::PutOp::PutOp(GSP_ProdCons &sync_data)
	: m_sync(sync_data)
{
	int	status;

	status = mutex_lock(&m_sync.m_mutex);
	assert(status == 0);
}



inline GSP_ProdCons::PutOp::~PutOp()
{
	int	status;

	status = mutex_unlock(&m_sync.m_mutex);
	assert(status == 0);

	status = sema_post(&m_sync.m_item_count);
	assert(status == 0);
}





//--------
// Inline implementation of class GSP_ProdCons::GetOp
//--------

inline GSP_ProdCons::GetOp::GetOp(GSP_ProdCons &sync_data)
	: m_sync(sync_data)
{
	int	status;

	status = sema_wait(&m_sync.m_item_count);
	assert(status == 0);

	status = mutex_lock(&m_sync.m_mutex);
	assert(status == 0);
}



inline GSP_ProdCons::GetOp::~GetOp()
{
	int	status;

	status = mutex_unlock(&m_sync.m_mutex);
	assert(status == 0);
}





//--------
// Inline implementation of class GSP_ProdCons::OtherOp
//--------

inline GSP_ProdCons::OtherOp::OtherOp(GSP_ProdCons &sync_data)
	: m_sync(sync_data)
{
	int	status;

	status = mutex_lock(&m_sync.m_mutex);
	assert(status == 0);
}



inline GSP_ProdCons::OtherOp::~OtherOp()
{
	int	status;

	status = mutex_unlock(&m_sync.m_mutex);
	assert(status == 0);
}





#endif
