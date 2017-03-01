#include <WinSock2.h>
#include <windows.h>
#include <process.h>
#include <Guiddef.h>
#include <vector>
#include <sstream>
#include <memory>

#include "ClassFactory.h"         
#include "Utils.h"
#include "Global.h"

const std::wstring kExtensionsName(L"ExampleShellExt");
std::unique_ptr<Global> g_Dll;
volatile bool g_ReadyToUnload = false;

struct CoTaskMemFreeGuard
{
    PVOID ptr;
    CoTaskMemFreeGuard(PVOID p): ptr(p){};
    ~CoTaskMemFreeGuard()
    {
        if (ptr != 0)
        {
            CoTaskMemFree(ptr);
        }
    }
};

static unsigned  __stdcall InitialThread(void *)
{
    PWSTR pFolderName = 0;
    g_ReadyToUnload = true;

    // init some resources here, e.g. logging
    return 0;
}

void DLL_Init()
{
    unsigned int tid = 0;
    HANDLE hThread = reinterpret_cast<HANDLE>(_beginthreadex(
        0,
        0,
        InitialThread,
        0,
        0,
        &tid));

    if (hThread > 0)
    {
        CloseHandle( hThread );
    }
}

void WaitForDll_InitToComplete()
{
    int attemptsCount = 10;
    int attemptsCounter = 0;
    while(!g_ReadyToUnload && attemptsCounter < 10)
    {
        Sleep(100);

        ++attemptsCounter;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {

    case DLL_PROCESS_ATTACH:
        {
            g_Dll.reset(new Global());
            g_Dll->DllInst(hModule);
            DLL_Init();
            break;
        }

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        {
            g_Dll.release();
            break;
        }
    }
    return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
    HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;
    std::vector<FactoryInfo> globalData = g_Dll->FactoryData();
    auto iter = globalData.begin();

    // to be sure, that the Dll_Init thread has already finished it's execution
    WaitForDll_InitToComplete();

    for(iter; iter != globalData.end(); ++iter) 
    {
        if(IsEqualCLSID(iter->m_CLSID, rclsid))
        {
            break;
        }
    }

    if(iter != globalData.end())
    {
        hr = E_OUTOFMEMORY;

        example::ClassFactory *pClassFactory = new example::ClassFactory(iter->m_Func);

        if (pClassFactory)
        {
            hr = pClassFactory->QueryInterface(riid, ppv);
            pClassFactory->Release();
        }
    }

    return hr;
}

STDAPI DllCanUnloadNow(void)
{
    // to be sure, that the Dll_Init thread has finished it's execution
    WaitForDll_InitToComplete();
    return g_Dll->DllRef() > 0 ? S_FALSE : S_OK;
}

STDAPI DllRegisterServer(void)
{
    HRESULT hr;
    int counter = 1;
    wchar_t szModule[MAX_PATH];

    // to be sure, that the Dll_Init thread has finished it's execution
    WaitForDll_InitToComplete();

    if (GetModuleFileName(g_Dll->DllInst(), szModule, ARRAYSIZE(szModule)) == 0)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    std::vector<FactoryInfo> factoryData = g_Dll->FactoryData();
    for(auto iter = factoryData.begin(); iter != factoryData.end(); ++iter)
    {
        hr = example::RegisterInprocServer(szModule, iter->m_CLSID, kExtensionsName.c_str(), L"Apartment");

        if (SUCCEEDED(hr))
        {
            if(iter == factoryData.end() - 1)
            {
                // register context menu extension for all types of files
                hr = example::RegisterShellExtContextMenuHandler(L"*", 
                    iter->m_CLSID, 
                    iter->m_friendlyName.c_str());
            }
            else
            {
                // register overlay icon extensions
                std::wostringstream ws;
                ws << "    ";
                ws << counter;
                ws << iter->m_friendlyName.c_str();

                hr = example::RegisterShellExtOverlayIconHandler(ws.str().c_str(), iter->m_CLSID);
            }
        }
        
        counter++;
    }

    return hr;
}

STDAPI DllUnregisterServer(void)
{
    HRESULT hr = S_OK;
    int counter = 1;
    wchar_t szModule[MAX_PATH];

    // to be sure, that the dll will not be unloaded before the inner thread finishes it's execution
    WaitForDll_InitToComplete();

    if (GetModuleFileName(g_Dll->DllInst(), szModule, ARRAYSIZE(szModule)) == 0)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }


    std::vector<FactoryInfo> factoryData = g_Dll->FactoryData();
    for(auto iter = factoryData.begin(); iter != factoryData.end(); ++iter)
    {
        hr = example::UnregisterInprocServer(iter->m_CLSID);

        if (SUCCEEDED(hr))
        {
            if( iter == factoryData.end() - 1)
            {
                // unregister context menu extension
                hr = example::UnregisterShellExtContextMenuHandler(L"*", 
                    iter->m_CLSID, 
                    iter->m_friendlyName.c_str());
            }
            else
            {
                // unregister overlay icon extensions
                std::wostringstream ws;
                ws << "    ";
                ws << counter;
                ws << iter->m_friendlyName.c_str();

                hr = example::UnregisterShellExtOverlayIconHandler(ws.str().c_str(), iter->m_CLSID);
            }
        }

        counter++;
    }

    return hr;
}
