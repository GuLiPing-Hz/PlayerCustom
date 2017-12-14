//------------------------------------------------------------------------------
// File: Util.h
//
// Desc: DirectShow sample code - simple helper macros
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#ifndef _UTIL_HH__
#define _UTIL_HH__

#define FAIL_RET(x) do { if( FAILED( hr = ( x  ) ) ) \
    return hr; } while(0)
#define FAIL_RET2(x) do { if( FAILED( hr = ( x  ) ) ) \
	return false; } while(0)
#define FAIL_RET3(x) do { if( FAILED( hr = ( x  ) ) ) \
	return; } while(0)
#define FAIL_GOTOFAILED(x) do { if( FAILED( hr = ( x  ) ) ) \
	goto failed; } while(0)
#endif // _UTIL_HH__