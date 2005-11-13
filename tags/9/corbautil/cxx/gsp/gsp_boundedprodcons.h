#ifdef __370__
#pragma nomargins
#endif

//-----------------------------------------------------------------------
// Copyright IONA Technologies 2002-2004. All rights reserved.
// This software is provided "as is".
//
// File:	gsp_boundedprodcons.h
//
// Policy:	BoundedProdCons(int size)[PutOp, GetOp, OtherOp]
//
// Description:	The producer-consumer synchronisation policy.
//----------------------------------------------------------------------


#ifndef GSP_BOUNDEDPRODCONS_H_
#define GSP_BOUNDEDPRODCONS_H_





#if defined(P_USE_WIN32_THREADS)
#	include "win_gsp_boundedprodcons.h"
#elif defined(P_USE_POSIX_THREADS)
#	include "posix_gsp_boundedprodcons.h"
#elif defined(P_USE_DCE_THREADS)
#	include "dce_gsp_boundedprodcons.h"
#elif defined(P_USE_SOLARIS_THREADS)
#	include "sol_gsp_boundedprodcons.h"
#elif defined(P_USE_TPF_THREADS)
#	error "No implemenation of class GSP_BoundedProdCons on TPF yet"
#elif defined(P_USE_NO_THREADS)
#	include "dummy_gsp_boundedprodcons.h"
#else
#	error "You must #define a P_USE_<platform>_THREADS symbol"
#endif





#endif
