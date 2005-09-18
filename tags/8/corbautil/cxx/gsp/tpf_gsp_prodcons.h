#ifdef __370__
#pragma nomargins
#endif

//-----------------------------------------------------------------------
// Copyright IONA Technologies 2002-2004. All rights reserved.
// This software is provided "as is".
//
// File:	tpf_gsp_prodcons.h
//
// Policy:	ProdCons[PutOp, GetOp, OtherOp]
//
// Description:	The producer-consumer synchronisation policy.
//----------------------------------------------------------------------


#ifndef TPF_GSP_PRODCONS_H_
#define TPF_GSP_PRODCONS_H_





//--------
// #include's
//--------
#include <tpfapi.h>
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
	double		m_lock;
	int		m_itemCount;
	int		m_consumerCount;
	ev0bk		m_wakeUpConsumers;
};





//--------
// Inline implementation of class GSP_ProdCons
//--------

inline GSP_ProdCons::GSP_ProdCons()
{
	int	status;

	m_itemCount = 0;
	m_consumerCount = 0;

	m_wakeUpConsumers.evnpstinf.evnbkc1 = 1;
	status = evntc(&m_wakeUpConsumers, EVENT_CNT, 'N', 0, EVNTC_1052);
	assert(status == 0);
}


inline GSP_ProdCons::~GSP_ProdCons()
{
	//--------
	// Nothing to do
	//--------
}





//--------
// Inline implementation of class GSP_ProdCons::PutOp
//--------

inline GSP_ProdCons::PutOp::PutOp(GSP_ProdCons &sync_data)
	: m_sync(sync_data)
{
	int	status;

	status = lockc((void *)&m_sync.m_lock, LOCK_O_SPIN);
	assert(status == 0);
}



inline GSP_ProdCons::PutOp::~PutOp()
{
	int	status;

	m_sync.m_itemCount ++;

	if (m_sync.m_consumerCount > 0) {
		//--------
		// Wake up blocked consumers
		//--------
		status = postc(&m_sync.m_wakeUpConsumers, EVENT_CNT, 0);
		assert(status == 0);

		//--------
		// Recreate the event.
		//--------
		m_sync.m_wakeUpConsumers.evnpstinf.evnbkc1 = 1;
		status = evntc(&m_sync.m_wakeUpConsumers,
					EVENT_CNT, 'N', 0, EVNTC_1052);
		assert(status == 0);
	}

	status = unlkc((void *)&m_sync.m_lock);
	assert(status == 0);
}





//--------
// Inline implementation of class GSP_ProdCons::GetOp
//--------

inline GSP_ProdCons::GetOp::GetOp(GSP_ProdCons &sync_data)
	: m_sync(sync_data)
{
	int	status;

	status = lockc((void *)&m_sync.m_lock, LOCK_O_SPIN);
	assert(status == 0);

	//--------
	// If necessary, wait until there are items in the buffer
	// so wait until there are
	//
	// pre-condition: we have the mutex lock
	//--------
	m_sync.m_consumerCount ++;
	while (m_sync.m_itemCount == 0) {
		status = unlkc((void *)&m_sync.m_lock);
		assert(status == 0);

		status = evnwc(&m_sync.m_wakeUpConsumers, EVENT_CNT);
		assert(status == 0);

		status = lockc((void *)&m_sync.m_lock, LOCK_O_SPIN);
		assert(status == 0);
	}

	//--------
	// post-condition: we have the mutex lock and there are items
	// in the buffer
	//--------
	assert(m_sync.m_itemCount > 0);
	m_sync.m_itemCount --;
	m_sync.m_consumerCount --;
}



inline GSP_ProdCons::GetOp::~GetOp()
{
	int	status;

	status = unlkc((void *)&m_sync.m_lock);
	assert(status == 0);
}





//--------
// Inline implementation of class GSP_ProdCons::OtherOp
//--------

inline GSP_ProdCons::OtherOp::OtherOp(GSP_ProdCons &sync_data)
	: m_sync(sync_data)
{
	int	status;

	status = lockc((void *)&m_sync.m_lock, LOCK_O_SPIN);
	assert(status == 0);
}



inline GSP_ProdCons::OtherOp::~OtherOp()
{
	int	status;

	status = unlkc((void *)&m_sync.m_lock);
	assert(status == 0);
}





#endif
