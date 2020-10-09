
#include "stdafx.h"
#include "Misc.h"
#include <Strsafe.h>

bool containsAtEnd(LPCWSTR wszContainer, LPCWSTR wszProspectiveEnding)
{
    size_t cchContainer = wcslen(wszContainer);
    size_t cchEnding = wcslen(wszProspectiveEnding);

    if (cchContainer < cchEnding)
    {
        return false;
    }

    if (cchEnding == 0)
    {
        return false;
    }

    if (_wcsicmp(wszProspectiveEnding, &(wszContainer[cchContainer - cchEnding])) != 0)
    {
        return false;
    }

    return true;
}

void outputDebugText(const wchar_t* format, ...)
{
    wchar_t buffer[4096];

    va_list vaList;
    va_start(vaList, format);
    StringCchVPrintfW(buffer, 4096, format, vaList);

    va_end(vaList);
    OutputDebugString(buffer);
}