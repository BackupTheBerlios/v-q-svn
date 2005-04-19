#ifdef __370__
#pragma nomargins
#endif

//-----------------------------------------------------------------------
// Copyright IONA Technologies 2002-2004. All rights reserved.
// This software is provided "as is".
//
// File:	gsp_mutex.h
//
// Policy:	Mutex[Op]	// non-recurisve mutex
//----------------------------------------------------------------------

#ifndef GSP_MUTEX_H_
#define GSP_MUTEX_H_





#if defined(P_USE_WIN32_THREADS)
#	include "win_gsp_mutex.h"
#elif defined(P_USE_POSIX_THREADS)
#	include "posix_gsp_mutex.h"
#elif defined(P_USE_DCE_THREADS)
#	include "dce_gsp_mutex.h"
#elif defined(P_USE_SOLARIS_THREADS)
#	include "sol_gsp_mutex.h"
#elif defined(P_USE_TPF_THREADS)
#	include "tpf_gsp_mutex.h"
#elif defined(P_USE_NO_THREADS)
#	include "dummy_gsp_mutex.h"
#else
#	error "You must #define a P_USE_<platform>_THREADS symbol"
#endif





#endif
