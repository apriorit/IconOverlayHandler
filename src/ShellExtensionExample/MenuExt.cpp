#include "MenuExt.h"
#include <stdio.h>
#include <strsafe.h>
#include <Shlwapi.h>
#include <string>
#include <assert.h>
#include <memory>
#include <algorithm>
#include <Shtypes.h>
#include <ShlObj.h>

#include "Global.h"
#include "resource.h"

const UINT kMaxQueryFileIndex = 0xFFFFFFFF;
extern std::unique_ptr<Global> g_Dll;


namespace example
{
    #define IDM_DISPLAY             0 

    MenuExt::MenuExt(void) : 
        m_cRef(1),
        m_menuText(L"Example Menu Item"),
        m_verb("exampletem"),
        m_wVerb(L"exampletem"),
        m_verbHelpText("Example Menu Item"),
        m_wVerbHelpText(L"Example Menu Item"),
        m_verbCanonicalName("ExampleItem"),
        m_wVerbCanonicalName(L"ExampleItem")
    {
         g_Dll->IncrementDllRef();
         m_hMenuBmp = LoadImage(g_Dll->DllInst(), MAKEINTRESOURCE(IDB_OK), 
             IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADTRANSPARENT);
    }

    MenuExt::~MenuExt(void)
    {
        g_Dll->DecrementDllRef();
    }

    HRESULT MenuExt::ComponentCreator(const IID& iid, void** ppv)
    {
        HRESULT hr = E_OUTOFMEMORY;

        MenuExt *pExt = new (std::nothrow) MenuExt();
        if (pExt)
        {
            hr = pExt->QueryInterface(iid, ppv);
            pExt->Release();
        }

        return hr;
    }

    void MenuExt::PerformAction()
    {
        // add logic to execute on the menu item click
    }

    #pragma region IUnknown

    IFACEMETHODIMP MenuExt::QueryInterface(REFIID riid, void **ppv)
    {
        static const QITAB qit[] = 
        {
            QITABENT(MenuExt, IContextMenu),
            QITABENT(MenuExt, IShellExtInit), 
            { 0 },
        };
        return QISearch(this, qit, riid, ppv);
    }

    IFACEMETHODIMP_(ULONG) MenuExt::AddRef()
    {
        return InterlockedIncrement(&m_cRef);
    }

    IFACEMETHODIMP_(ULONG) MenuExt::Release()
    {
        ULONG cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0)
        {
            delete this;
        }

        return cRef;
    }

    #pragma endregion


    #pragma region IShellExtInit

    IFACEMETHODIMP MenuExt::Initialize(
        LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID)
    {
        if (pDataObj == NULL)
        {
            return E_INVALIDARG;
        }

        HRESULT hr = E_FAIL;
        
        FORMATETC fe = { (CLIPFORMAT)g_Dll->ShellIdList(), NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        STGMEDIUM stm;

        if (SUCCEEDED(pDataObj->GetData(&fe, &stm)))
        {
            FORMATETC etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
            STGMEDIUM stg = { TYMED_HGLOBAL };

            if ( SUCCEEDED( pDataObj->GetData ( &etc, &stg )))
            {
                HDROP hDrop = static_cast<HDROP>(GlobalLock(stg.hGlobal));
                if (hDrop != NULL)
                {
                    UINT nFiles = DragQueryFileW(hDrop, kMaxQueryFileIndex, NULL, 0);      
                    // some logic to process selected items
                    hr = S_OK;

                    GlobalUnlock(stg.hGlobal);

                    ReleaseStgMedium(&stg);
                }
            }

            ReleaseStgMedium(&stm);
        }

        return hr;
    }

    #pragma endregion

    #pragma region IContextMenu

    IFACEMETHODIMP MenuExt::QueryContextMenu(
        HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
    {
        if (CMF_DEFAULTONLY & uFlags || CMF_NOVERBS & uFlags || CMF_VERBSONLY & uFlags)
        {
            return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));
        }

        MENUITEMINFO item = { sizeof(MENUITEMINFO) };

        item.fMask = MIIM_BITMAP | MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_STATE;
        item.wID = idCmdFirst + IDM_DISPLAY;
        item.fType = MFT_STRING;
        item.fState = MFS_ENABLED;
        item.dwTypeData = &m_menuText[0];
        item.hbmpItem = static_cast<HBITMAP>(m_hMenuBmp);

        if (!InsertMenuItem(hMenu, indexMenu, TRUE, &item))
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        MENUITEMINFO sep = { sizeof(sep) };
        sep.fMask = MIIM_TYPE;
        sep.fType = MFT_SEPARATOR;
        if (!InsertMenuItem(hMenu, indexMenu + 1, TRUE, &sep))
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(IDM_DISPLAY + 1));
    }

    IFACEMETHODIMP MenuExt::InvokeCommand(LPCMINVOKECOMMANDINFO pici)
    {
        BOOL fUnicode = FALSE;
        int commandID = -1;

        if (pici->cbSize == sizeof(CMINVOKECOMMANDINFOEX))
        {
            if (pici->fMask & CMIC_MASK_UNICODE)
            {
                fUnicode = TRUE;
            }
        }

        if (!fUnicode && HIWORD(pici->lpVerb))
        {
            if (StrCmpIA(pici->lpVerb, &m_verb[0]) == 0)
            {
                PerformAction();
            }
            else
            {
                return E_FAIL;
            }
        }
        else if (fUnicode && HIWORD(((CMINVOKECOMMANDINFOEX*)pici)->lpVerbW))
        {
            if (StrCmpIW(((CMINVOKECOMMANDINFOEX*)pici)->lpVerbW, &m_wVerb[0]) == 0)
            {
                PerformAction();
            }
            else
            {
                return E_FAIL;
            }
        }
        else
        {
            if (LOWORD(pici->lpVerb) == IDM_DISPLAY)
            {
                PerformAction();
            }
            else
            {
                return E_FAIL;
            }
        }

        return S_OK;

    }

    IFACEMETHODIMP MenuExt::GetCommandString(UINT_PTR idCommand, 
        UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax)
    {
        switch (uFlags)
        {
        case GCS_HELPTEXTW:
            StringCchCopy(reinterpret_cast<PWSTR>(pszName), cchMax, m_wVerbHelpText.c_str());
            break;

        case GCS_VERBW:
            StringCchCopy(reinterpret_cast<PWSTR>(pszName), cchMax, m_wVerbCanonicalName.c_str());
            break;

        default:
            return S_OK;
        }

        return S_OK;
    }

    #pragma endregion
}

