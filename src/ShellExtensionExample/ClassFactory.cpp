#include "ClassFactory.h"
#include <new>
#include <Shlwapi.h>
#include <memory>

#include "MenuExt.h"

extern std::unique_ptr<Global> g_Dll;

namespace example
{
    ClassFactory::ClassFactory(FPCOMPCREATOR pFuncTemp) : m_cRef(1), pCreator(pFuncTemp)
    {
        g_Dll->IncrementDllRef();
    }

    ClassFactory::~ClassFactory()
    {
        g_Dll->DecrementDllRef();
    }


    //
    // IUnknown
    //

    IFACEMETHODIMP ClassFactory::QueryInterface(REFIID riid, void **ppv)
    {
        static const QITAB qit[] = 
        {
            QITABENT(ClassFactory, IClassFactory),
            { 0 },
        };
        return QISearch(this, qit, riid, ppv);
    }

    IFACEMETHODIMP_(ULONG) ClassFactory::AddRef()
    {
        return InterlockedIncrement(&m_cRef);
    }

    IFACEMETHODIMP_(ULONG) ClassFactory::Release()
    {
        ULONG cRef = InterlockedDecrement(&m_cRef);
        if (0 == cRef)
        {
            delete this;
        }
        return cRef;
    }


    // 
    // IClassFactory
    //

    IFACEMETHODIMP ClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv)
    {
        HRESULT hr = CLASS_E_NOAGGREGATION;

        if (pUnkOuter == NULL)
        {
            hr = E_OUTOFMEMORY;

            hr = (*pCreator)(riid, ppv);
        }

        return hr;
    }

    IFACEMETHODIMP ClassFactory::LockServer(BOOL fLock)
    {
        if (fLock)
        {
            g_Dll->IncrementDllRef();
        }
        else
        {
            g_Dll->DecrementDllRef();
        }

        return S_OK;
    }
}