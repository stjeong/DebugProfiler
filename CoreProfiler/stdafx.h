// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED

#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit


#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "math.h"
#include "time.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "limits.h"
#include "malloc.h"
#include "string.h"

#include <windows.h>
#include <assert.h>
#include "winreg.h"
#include "wincrypt.h"
#include "winbase.h"
#include "objbase.h"

#include "cor.h"
#include "corhdr.h"
#include "corhlpr.h"
#include "corerror.h"

#include "corpub.h"
#include "corprof.h"
#include "cordebug.h"
#include "SpecStrings.h"
#include "mscoree.h"


#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

#include <corprof.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>