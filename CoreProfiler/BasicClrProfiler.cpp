// BasicClrProfiler.cpp : Implementation of CBasicClrProfiler

#include "stdafx.h"
#include "BasicClrProfiler.h"

#include "ClrModule.h"
#include "Constants.h"

// CBasicClrProfiler

HRESULT CBasicClrProfiler::Initialize(IUnknown * pICorProfilerInfoUnk)
{
	if (pICorProfilerInfoUnk == NULL)
	{
		return S_OK;
	}

	m_pICorProfilerInfo2 = pICorProfilerInfoUnk;

	DWORD dwEventMask = COR_PRF_MONITOR_MODULE_LOADS | COR_PRF_MONITOR_JIT_COMPILATION 
        | COR_PRF_DISABLE_INLINING | COR_PRF_USE_PROFILE_IMAGES;
	m_pICorProfilerInfo2->SetEventMask(dwEventMask);

    copyInteropHelperDll();

	return S_OK;
}

void CBasicClrProfiler::copyInteropHelperDll()
{
    wchar_t exeFilePath[MAX_PATH];
    wchar_t dllFilePath[MAX_PATH];

    if (GetModuleFileName(nullptr, exeFilePath, MAX_PATH) == 0)
    {
        return;
    }

    HINSTANCE hInst = (HINSTANCE)_AtlBaseModule.m_hInst;
    if (GetModuleFileName(hInst, dllFilePath, MAX_PATH) == 0)
    {
        return;
    }

    ::PathRemoveFileSpec(exeFilePath);
    ::PathRemoveFileSpec(dllFilePath);

    ::PathCombine(exeFilePath, exeFilePath, NAME_HELPER_DLL);
    ::PathCombine(dllFilePath, dllFilePath, NAME_HELPER_DLL);

    ::CopyFile(dllFilePath, exeFilePath, FALSE);
}

HRESULT CBasicClrProfiler::ModuleLoadFinished(ModuleID moduleId, HRESULT hrStatus)
{
    ClrModule clrModule(m_pICorProfilerInfo2, moduleId);

    if (clrModule.Initialize() == false)
    {
        return S_OK;
    }

    ModuleContext context;
    clrModule.PrepareModuleContext(context);

    m_moduleIDToInfoMap.Update(moduleId, context);    
	return S_OK;
}

HRESULT CBasicClrProfiler::ModuleUnloadStarted(ModuleID moduleId)
{
    m_moduleIDToInfoMap.EraseIfExists(moduleId);
    return S_OK;
}

HRESULT CBasicClrProfiler::JITCompilationStarted(FunctionID functionId, BOOL fIsSafeToBlock)
{
    mdToken methodToken = 0;
    ModuleID moduleId = 0;
    ClassID classId;

    HRESULT hr = m_pICorProfilerInfo2->GetFunctionInfo(functionId, &classId, &moduleId, &methodToken);
    if (hr != S_OK || methodToken == 0)
    {
        return S_OK;
    }

    ModuleContext context;
    if (m_moduleIDToInfoMap.LookupIfExists(moduleId, &context) == FALSE)
    {
        return S_OK;
    }

    ClrModule clrModule(m_pICorProfilerInfo2, moduleId);

    clrModule.Rewrite(context, classId, functionId, methodToken);

    return S_OK;
}
