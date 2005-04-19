#ifdef __370__
#pragma nomargins
#endif

//-----------------------------------------------------------------------
// Copyright IONA Technologies 2002-2004. All rights reserved.
// This software is provided "as is".
//
// File:	gsp_prodcons.h
//
// Policy:	ProdCons[PutOp, GetOp, OtherOp]
//
// Description:	The producer-consumer synchronisation policy.
//----------------------------------------------------------------------


#ifndef GSP_PRODCONS_H_
#define GSP_PRODCONS_H_





#if defined(P_USE_WIN32_THREADS)
#	include "win_gsp_prodcons.h"
#elif defined(P_USE_POSIX_THREADS)
#	include "posix_gsp_prodcons.h"
#elif defined(P_USE_DCE_THREADS)
#	include "dce_gsp_prodcons.h"
#elif defined(P_USE_SOLARIS_THREADS)
#	include "sol_gsp_prodcons.h"
#elif defined(P_USE_TPF_THREADS)
#	include "tpf_gsp_prodcons.h"
#elif defined(P_USE_NO_THREADS)
#	include "dummy_gsp_prodcons.h"
#else
#	error "You must #define a P_USE_<platform>_THREADS symbol"
#endif





#endif
