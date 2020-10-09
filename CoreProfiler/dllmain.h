// dllmain.h : Declaration of module class.

class CCoreProfilerModule : public ATL::CAtlDllModuleT< CCoreProfilerModule >
{
public :
	DECLARE_LIBID(LIBID_CoreProfilerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_COREPROFILER, "{0D2A0B57-DF88-4B2D-BB08-1976D8573E17}")
};

extern class CCoreProfilerModule _AtlModule;
