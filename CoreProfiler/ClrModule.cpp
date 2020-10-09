
#include "stdafx.h"
#include "ClrModule.h"
#include "Misc.h"
#include "Constants.h"
#include "ILRewriter.h"

bool ClrModule::Initialize()
{
    HRESULT hr;

    if (canApply() == false)
    {
        return false;
    }

    hr = m_pICorProfilerInfo2->GetModuleMetaData(m_moduleId, (ofRead | ofWrite), IID_IMetaDataAssemblyImport, (LPUNKNOWN *)&m_pMetaDataAssemblyImport);
    if (hr != S_OK)
    {
        return false;
    }

    hr = m_pMetaDataAssemblyImport->QueryInterface(IID_IMetaDataAssemblyEmit, (LPVOID *)&m_pAssemblyEmit);
    if (hr != S_OK)
    {
        return false;
    }

    hr = m_pMetaDataAssemblyImport->QueryInterface(IID_IMetaDataEmit, (LPVOID *)&m_pEmit);
    if (hr != S_OK)
    {
        return false;
    }

    hr = m_pMetaDataAssemblyImport->QueryInterface(IID_IMetaDataImport, (LPVOID *)&m_pMetaDataImport);
    if (hr != S_OK)
    {
        return false;
    }

    return true;
}

bool ClrModule::retrieveObjectToken(ModuleContext &context)
{
    mdToken tkObject = mdTokenNil;

    wchar_t *objectTypeName = L"System.Object";
    m_pMetaDataImport->FindTypeDefByName(objectTypeName, 0, &tkObject);
    if (IsNilToken(tkObject) == true)
    {
        tkObject = getTypeRef(objectTypeName);
    }

    if (IsNilToken(tkObject) == true)
    {
        return false;
    }

    context.m_mdObjectToken = tkObject;
    return true;
}

bool ClrModule::makeHelperAssemblyRef(ModuleContext &context)
{
    WCHAR wszLocale[] = { L"neutral" };

    ASSEMBLYMETADATA assemblyMetaData;
    ZeroMemory(&assemblyMetaData, sizeof(assemblyMetaData));
    assemblyMetaData.usMajorVersion = 1;
    assemblyMetaData.usMinorVersion = 0;
    assemblyMetaData.usBuildNumber = 0;
    assemblyMetaData.usRevisionNumber = 0;
    assemblyMetaData.szLocale = wszLocale;
    assemblyMetaData.cbLocale = _countof(wszLocale);

    mdAssemblyRef assemblyRef = NULL;
    HRESULT hr = m_pAssemblyEmit->DefineAssemblyRef((void *)g_rgbPublicKeyToken, sizeof(g_rgbPublicKeyToken),
        NAME_HELPER_ASSEMBLY, &assemblyMetaData, NULL, NULL, 0, &assemblyRef);
    if (hr != S_OK || IsNilToken(assemblyRef))
    {
        return false;
    }

    mdTypeRef typeRef = mdTokenNil;
    hr = m_pEmit->DefineTypeRefByName(assemblyRef, NAME_HELPER_MANAGEDTYPE, &typeRef);
    if (hr != S_OK || IsNilToken(typeRef))
    {
        return false;
    }

    COR_SIGNATURE sigFunctionProbe[] = {
        IMAGE_CEE_CS_CALLCONV_DEFAULT,      // default calling convention
        0x2,                                // number of arguments == 1
        ELEMENT_TYPE_VOID,                  // return type == void
        ELEMENT_TYPE_OBJECT,                // 1st arg type == System.Object
        ELEMENT_TYPE_SZARRAY,               // 2nd arg type == System.Object []
        ELEMENT_TYPE_OBJECT,
    };

    mdToken mdEnterProbeRef;
    hr = m_pEmit->DefineMemberRef(typeRef, NAME_HELPER_METHOD_ENTER,
        sigFunctionProbe, sizeof(sigFunctionProbe), &mdEnterProbeRef);
    if (hr != S_OK)
    {
        return false;
    }

    context.m_mdEnterProbeRef = mdEnterProbeRef;

    return true;
}

bool ClrModule::makePrimitiveTypeRef(ModuleContext &context)
{
    for (int i = ELEMENT_TYPE_BOOLEAN; i < (ELEMENT_TYPE_BOOLEAN + PRIMITIVE_COUNT); i++)
    {
        wchar_t *typeName = m_primitiveNames[i - ELEMENT_TYPE_BOOLEAN];

        mdToken token = getTypeTokenByName(typeName);
        if (IsNilToken(token) == true)
        {
            token = tryToMakeTypeReference(L"mscorlib", typeName);
        }

        if (IsNilToken(token) == true)
        {
            return false;
        }

        context.m_primitives[i] = token;
    }

    return true;
}

mdToken ClrModule::getTypeTokenByName(const wchar_t *typeName)
{
    if (wcscmp(typeName, L"System.Void") == 0)
    {
        return 0;
    }

    mdTypeDef td = 0;
    HRESULT hr = m_pMetaDataImport->FindTypeDefByName(typeName, 0, &td);
    if (IsNilToken(td) == false)
    {
        return td;
    }

    HCORENUM corEnum = 0;
    mdAssemblyRef mdAssemblies[MAX_LOOKUP_OF_ASMREF];
    DWORD cAssemblyRefs = 0;

    mdTypeRef foundTypeRef = 0;

    do
    {
        hr = m_pMetaDataAssemblyImport->EnumAssemblyRefs(&corEnum, mdAssemblies, MAX_LOOKUP_OF_ASMREF, &cAssemblyRefs);
        if (S_OK != hr)
        {
            break;
        }

        mdTypeRef tr = 0;
        for (ULONG asmIndex = 0; asmIndex < cAssemblyRefs; asmIndex++)
        {
            hr = m_pMetaDataImport->FindTypeRef(mdAssemblies[asmIndex], typeName, &tr);
            if (hr == S_OK)
            {
                foundTypeRef = tr;
                break;
            }
        }

    } while (IsNilToken(foundTypeRef));

    if (corEnum != 0)
    {
        m_pMetaDataAssemblyImport->CloseEnum(corEnum);
    }

    return foundTypeRef;
}

mdToken ClrModule::tryToMakeTypeReference(const wchar_t *assemblyName, const wchar_t *typeName)
{
    mdToken typeToken = mdTokenNil;

    HCORENUM assemblyEnum = 0;
    mdAssemblyRef asmRefs[MAX_LOOKUP_OF_ASMREF];
    ULONG cTokens;
    bool foundAssembly = false;

    do
    {

        HRESULT hr = m_pMetaDataAssemblyImport->EnumAssemblyRefs(&assemblyEnum, asmRefs, MAX_LOOKUP_OF_ASMREF, &cTokens);
        if (hr != S_OK)
        {
            break;
        }

        wchar_t wchName[MAX_ASSEMBLY_NAME_BUF];
        ULONG chName;

        for (ULONG i = 0; i < cTokens; i++)
        {
            mdAssemblyRef asmRef = asmRefs[i];
            hr = m_pMetaDataAssemblyImport->GetAssemblyRefProps(asmRef, nullptr, nullptr,
                wchName, MAX_ASSEMBLY_NAME_BUF, &chName, nullptr, nullptr, nullptr, nullptr);

            if (hr != S_OK)
            {
                continue;
            }

            foundAssembly = _wcsicmp(wchName, assemblyName) == 0;
            if (foundAssembly)
            {
                m_pEmit->DefineTypeRefByName(asmRef, typeName, &typeToken);
                break;
            }
        }

    } while (foundAssembly == false);

    if (assemblyEnum != 0)
    {
        m_pMetaDataAssemblyImport->CloseEnum(assemblyEnum);
    }

    return typeToken;
}

bool ClrModule::PrepareModuleContext(ModuleContext &moduleContext)
{
    if (retrieveObjectToken(moduleContext) == false)
    {
        return false;
    }

    if (makeHelperAssemblyRef(moduleContext) == false)
    {
        return false;
    }

    if (makePrimitiveTypeRef(moduleContext) == false)
    {
        return false;
    }

    return true;
}

void ClrModule::Rewrite(ModuleContext &context, ClassID classId, FunctionID functionId, mdToken methodToken)
{
    if (context.IsValid() == false)
    {
        return;
    }

    RewriteIL(m_pICorProfilerInfo2, m_moduleId, methodToken, context);
}

mdTypeRef ClrModule::getTypeRef(const wchar_t *findTypeName)
{
    const int maxTokens = 512;
    ULONG typeRefCount = 0;
    HCORENUM hEnum = 0;
    mdTypeRef rTypeRefs[maxTokens];
    mdTypeRef foundRef = mdTypeRefNil;

    do
    {

        HRESULT hr = m_pMetaDataImport->EnumTypeRefs(&hEnum, rTypeRefs, maxTokens, &typeRefCount);
        if (S_OK != hr)
        {
            break;
        }

        wchar_t wszTypeName[MAX_PATH];

        for (ULONG typeIndex = 0; typeIndex < typeRefCount; typeIndex++)
        {
            ULONG cbTypeName;
            hr = m_pMetaDataImport->GetTypeRefProps(rTypeRefs[typeIndex], 0, wszTypeName, MAX_PATH, &cbTypeName);
            if (S_OK != hr)
            {
                continue;
            }

            if (wcscmp(findTypeName, wszTypeName) == 0)
            {
                foundRef = rTypeRefs[typeIndex];
                break;
            }
        }

    } while (IsNilToken(foundRef) == true);

    if (hEnum != 0)
    {
        m_pMetaDataImport->CloseEnum(hEnum);
    }

    return foundRef;
}

void ClrModule::retrieveModuleName()
{
    ULONG cchModule = _MAX_PATH;
    ULONG rCchModule = 0;

    m_pICorProfilerInfo2->GetModuleInfo(m_moduleId, nullptr, cchModule, &rCchModule, m_szModule, nullptr);
}

bool ClrModule::canApply()
{
    retrieveModuleName();

    if (containsAtEnd(m_szModule, NAME_MSCORLIB_DLL) || containsAtEnd(m_szModule, NAME_HELPER_DLL))
    {
        return false;
    }

    return true;
}