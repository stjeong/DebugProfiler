#pragma once

#include "stdafx.h"


class CSHolder
{
public:
    CSHolder(CRITICAL_SECTION * pcs)
    {
        m_pcs = pcs;
        EnterCriticalSection(m_pcs);
    }

    ~CSHolder()
    {
        assert(m_pcs != NULL);
        LeaveCriticalSection(m_pcs);
    }

private:
    CRITICAL_SECTION * m_pcs;
};


template <class _ID, class _Info>
class IDToInfoMap
{
public:
    typedef std::map<_ID, _Info> Map;
    typedef typename Map::iterator Iterator;
    typedef typename Map::const_iterator Const_Iterator;
    typedef typename Map::size_type Size_type;

    IDToInfoMap()
    {
        InitializeCriticalSection(&m_cs);
    }

    Size_type GetCount()
    {
        CSHolder csHolder(&m_cs);
        return m_map.size();
    }

    BOOL LookupIfExists(_ID id, _Info * pInfo)
    {
        CSHolder csHolder(&m_cs);
        Const_Iterator iterator = m_map.find(id);
        if (iterator == m_map.end())
        {
            return FALSE;
        }

        *pInfo = iterator->second;
        return TRUE;
    }

    _Info Lookup(_ID id)
    {
        CSHolder csHolder(&m_cs);
        _Info info;
        LookupIfExists(id, &info);

        return info;
    }

    void EraseIfExists(_ID id)
    {
        CSHolder csHolder(&m_cs);

        Const_Iterator iterator = m_map.find(id);
        if (iterator == m_map.end())
        {
            return;
        }

        m_map.erase(id);
    }

    void Erase(_ID id)
    {
        CSHolder csHolder(&m_cs);
        m_map.erase(id);
    }

    void Update(_ID id, _Info info)
    {
        CSHolder csHolder(&m_cs);
        m_map[id] = info;
    }

    Const_Iterator Begin()
    {
        return m_map.begin();
    }

    Const_Iterator End()
    {
        return m_map.end();
    }

    class LockHolder
    {
    public:
        LockHolder(IDToInfoMap<_ID, _Info> * pIDToInfoMap) :
            m_csHolder(&(pIDToInfoMap->m_cs))
        {
        }

    private:
        CSHolder m_csHolder;
    };

private:
    Map m_map;
    CRITICAL_SECTION m_cs;
};

#define PRIMITIVE_COUNT (ELEMENT_TYPE_R8 - ELEMENT_TYPE_BOOLEAN + 1)
struct ModuleContext
{
    mdToken m_mdEnterProbeRef = 0;
    mdToken m_mdObjectToken = 0;

    mdToken m_primitives[ELEMENT_TYPE_MAX];

    bool IsValid()
    {
        return IsNilToken(m_mdEnterProbeRef) == false &&
            IsNilToken(m_mdObjectToken) == false;
    }
};

typedef IDToInfoMap<ModuleID, ModuleContext> ModuleIDToInfoMap;