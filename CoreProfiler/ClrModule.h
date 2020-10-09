#pragma once

#include "ProfilerData.h"

using namespace ATL;

class ClrModule
{
private:
    IMetaDataAssemblyImport *m_pMetaDataAssemblyImport = nullptr;
    IMetaDataAssemblyEmit *m_pAssemblyEmit = nullptr;
    IMetaDataEmit *m_pEmit = nullptr;
    IMetaDataImport *m_pMetaDataImport = nullptr;

    CComQIPtr<ICorProfilerInfo2> m_pICorProfilerInfo2;
    ModuleID m_moduleId = 0;
    wchar_t m_szModule[_MAX_PATH];

    void retrieveModuleName();
    bool canApply();
    mdTypeRef getTypeRef(const wchar_t *findTypeName);
    bool retrieveObjectToken(ModuleContext &context);
    bool makeHelperAssemblyRef(ModuleContext &context);
    bool makePrimitiveTypeRef(ModuleContext &context);
    mdToken getTypeTokenByName(const wchar_t *typeName);
    mdToken tryToMakeTypeReference(const wchar_t *assemblyName, const wchar_t *typeName);

    wchar_t *m_primitiveNames[PRIMITIVE_COUNT] = {
        L"System.Boolean",
        L"System.Char",
        L"System.SByte",
        L"System.Byte",
        L"System.Int16",
        L"System.UInt16",
        L"System.Int32",
        L"System.UInt32",
        L"System.Int64",
        L"System.UInt64",
        L"System.Single",
        L"System.Double",
    };

public:
    ClrModule(ICorProfilerInfo2 *pICorProfilerInfo2, ModuleID moduleId)
    {
        m_pICorProfilerInfo2 = pICorProfilerInfo2;
        m_moduleId = moduleId;
    }

    ~ClrModule()
    {
        if (m_pMetaDataAssemblyImport != nullptr)
        {
            m_pMetaDataAssemblyImport->Release();
        }

        if (m_pAssemblyEmit != nullptr)
        {
            m_pAssemblyEmit->Release();
        }

        if (m_pEmit != nullptr)
        {
            m_pEmit->Release();
        }

        if (m_pMetaDataImport != nullptr)
        {
            m_pMetaDataImport->Release();
        }
    }

    bool Initialize();
    bool PrepareModuleContext(ModuleContext &moduleContext);
    void Rewrite(ModuleContext &context, ClassID classId, FunctionID functionId, mdToken methodToken);
};
