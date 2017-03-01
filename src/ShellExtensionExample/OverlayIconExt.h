#ifndef ENCRYPTED_OVERLAY_ICON_EXT_H
#define ENCRYPTED_OVERLAY_ICON_EXT_H

#include <WinSock2.h>
#include <windows.h>
#include <shlobj.h>
#include <string>


namespace example
{
    class OverlayIconExt : IShellIconOverlayIdentifier 
    {
    public:
        // IUnknown
        IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();

        //IShellIconOverlayIdentifier
        STDMETHOD(GetOverlayInfo)(LPWSTR pwszIconFile, int cchMax, int *pIndex, DWORD* pdwFlags);
        STDMETHOD(GetPriority)(int* pPriority);
        STDMETHOD(IsMemberOf)(LPCWSTR pwszPath, DWORD dwAttrib);

        static HRESULT ComponentCreator(const IID& iid, void** ppv);

        OverlayIconExt(void);

    protected:
        ~OverlayIconExt(void);

    private:
        long m_cRef;
    };

}

#endif