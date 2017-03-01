#ifndef UTILS_H
#define UTILS_H

#include <WinSock2.h>
#include <windows.h>

namespace example
{
    HRESULT RegisterInprocServer(PCWSTR pszModule, const CLSID& clsid, 
        PCWSTR pszFriendlyName, PCWSTR pszThreadModel);

    HRESULT UnregisterInprocServer(const CLSID& clsid);

    HRESULT RegisterShellExtOverlayIconHandler(
        PCWSTR pszName, const CLSID& clsid);

    HRESULT UnregisterShellExtOverlayIconHandler(
        PCWSTR pszName, const CLSID& clsid);

    HRESULT RegisterShellExtContextMenuHandler(
        PCWSTR pszFileType, const CLSID& clsid, PCWSTR pszFriendlyName);

    HRESULT UnregisterShellExtContextMenuHandler(PCWSTR pszFileType, const CLSID& clsid,
        PCWSTR pszFriendlyName);

}

#endif UTILS_H