#include "OverlayIconExt.h"
#include <strsafe.h>
#include <Shlwapi.h>
#include <memory>

#include "Global.h"

extern std::unique_ptr<Global> g_Dll;

namespace example
{
    OverlayIconExt::OverlayIconExt(void) : m_cRef(1)
    {
        g_Dll->IncrementDllRef();
    }

    OverlayIconExt::~OverlayIconExt(void)
    {
         g_Dll->DecrementDllRef();
    }

    #pragma region IUnknown

    IFACEMETHODIMP OverlayIconExt::QueryInterface(REFIID riid, void **ppv)
    {
        static const QITAB qit[] = 
        {
            QITABENT(OverlayIconExt, IShellIconOverlayIdentifier),
            { 0 },
        };
        return QISearch(this, qit, riid, ppv);
    }

    IFACEMETHODIMP_(ULONG) OverlayIconExt::AddRef()
    {
        return InterlockedIncrement(&m_cRef);
    }

    IFACEMETHODIMP_(ULONG) OverlayIconExt::Release()
    {
        ULONG cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0)
        {
            delete this;
        }
        return cRef;
    }

    #pragma endregion

    #pragma region IShellIconOverlayIdentifier

    STDMETHODIMP OverlayIconExt::GetOverlayInfo(LPWSTR pwszIconFile, int cchMax,
        int* pIndex, DWORD* pdwFlags)
    {
        if(pwszIconFile == 0)
        {
            return E_POINTER;
        }
        if(pIndex == 0)
        {
            return E_POINTER;
        }
        if(pdwFlags == 0)
        {
            return E_POINTER;
        }
        if(cchMax < 1)
        {
            return E_INVALIDARG;
        }

        GetModuleFileNameW(g_Dll->DllInst(), pwszIconFile, cchMax);

        // index of icon to use in list of resources
        *pIndex = 0;
        *pdwFlags = ISIOI_ICONFILE| ISIOI_ICONINDEX;
       
        return S_OK;
    }

    STDMETHODIMP OverlayIconExt::GetPriority(int* pPriority)
    {
        if(*pPriority)
        {
            return E_INVALIDARG;
        }

        *pPriority = 0;
        return S_OK;
    }

    STDMETHODIMP OverlayIconExt::IsMemberOf(LPCWSTR pwszPath, DWORD dwAttrib)
    {
        // here some logic to determine whether to show overlay icon can be placed

        return S_OK;
    }

    #pragma endregion

    HRESULT OverlayIconExt::ComponentCreator(const IID& iid, void** ppv)
    {
        HRESULT hr = E_OUTOFMEMORY;

        OverlayIconExt *pExt = new (std::nothrow) OverlayIconExt();
        if (pExt)
        {
            hr = pExt->QueryInterface(iid, ppv);
            pExt->Release();
        }

        return hr;
    }

}
