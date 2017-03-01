#ifndef CONTEXT_MENU_EXT_H
#define CONTEXT_MENU_EXT_H

#include <WinSock2.h>
#include <windows.h>
#include <map>
#include <vector>
#include <memory>
#include <shlobj.h>

namespace example
{
    class MenuExt: public IShellExtInit, public IContextMenu
    {
    public:
        // IUnknown
        IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();

        // IShellExtInit
        IFACEMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID);

        // IContextMenu
        IFACEMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
        IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici);
        IFACEMETHODIMP GetCommandString(UINT_PTR idCommand, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax);

        static HRESULT ComponentCreator(const IID& iid, void** ppv);

        MenuExt(void);
        ~MenuExt(void);

    private:
        void PerformAction();

    private:
        long m_cRef;
        std::wstring m_menuText;
        std::string m_verb;
        std::wstring m_wVerb;
        std::string m_verbHelpText;
        std::wstring m_wVerbHelpText;
        std::string m_verbCanonicalName;
        std::wstring m_wVerbCanonicalName;
        HANDLE m_hMenuBmp;
    };

}

#endif CONTEXT_MENU_EXT_H