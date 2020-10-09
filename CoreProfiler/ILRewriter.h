#pragma once

#include "stdafx.h"

extern HRESULT RewriteIL(ICorProfilerInfo2 * pICorProfilerInfo, ModuleID moduleID,
    mdMethodDef methodDef, ModuleContext &moduleInfo);