// BasicClrProfiler.h : Declaration of the CBasicClrProfiler

#pragma once
#include "resource.h"       // main symbols

#include "CoreProfiler_i.h"

#include "ProfilerData.h"
#include "ICorProfilerCallback3Impl.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

// CBasicClrProfiler

class ATL_NO_VTABLE CBasicClrProfiler :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CBasicClrProfiler, &CLSID_BasicClrProfiler>,
	public IDispatchImpl<IBasicClrProfiler, &IID_IBasicClrProfiler, &LIBID_CoreProfilerLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public ICorProfilerCallback3Impl<CBasicClrProfiler>
{
public:
	CBasicClrProfiler()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_BASICCLRPROFILER)

BEGIN_COM_MAP(CBasicClrProfiler)
	COM_INTERFACE_ENTRY(IBasicClrProfiler)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ICorProfilerCallback)
	COM_INTERFACE_ENTRY(ICorProfilerCallback2)
	COM_INTERFACE_ENTRY(ICorProfilerCallback3)
END_COM_MAP()

    STDMETHOD(Initialize)(IUnknown * pICorProfilerInfoUnk);
    STDMETHOD(ModuleLoadFinished)(ModuleID moduleId, HRESULT hrStatus);
    STDMETHOD(ModuleUnloadStarted)(ModuleID moduleId);
    STDMETHOD(JITCompilationStarted)(FunctionID functionId, BOOL fIsSafeToBlock);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

private:
    IDToInfoMap<ModuleID, ModuleContext> m_moduleIDToInfoMap;

    CComQIPtr<ICorProfilerInfo2> m_pICorProfilerInfo2;
    void copyInteropHelperDll();
};

OBJECT_ENTRY_AUTO(__uuidof(BasicClrProfiler), CBasicClrProfiler)
