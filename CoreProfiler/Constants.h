#pragma once

constexpr const wchar_t *NAME_HELPER_DLL = L"Intercept.Helper.dll";
constexpr const wchar_t *NAME_HELPER_ASSEMBLY = L"Intercept.Helper";
constexpr const wchar_t *NAME_HELPER_MANAGEDTYPE = L"Intercept.Helper.ManagedLayer";
constexpr const wchar_t *NAME_HELPER_METHOD_ENTER = L"Enter";
constexpr const wchar_t *NAME_MSCORLIB_DLL = L"mscorlib.dll";
constexpr const BYTE g_rgbPublicKeyToken[] = { 0x20, 0xa5, 0x97, 0x60, 0x64, 0xab, 0x52, 0x7b };

constexpr const int MAX_LOOKUP_OF_ASMREF = 32;
constexpr const int MAX_LOOKUP_OF_TYPESPEC = 64;

constexpr const int MAX_ASSEMBLY_NAME_BUF = 1024;