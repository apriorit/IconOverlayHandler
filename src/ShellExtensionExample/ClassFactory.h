#ifndef CLASS_FACTORY_H
#define CLASS_FACTORY_H


#include <WinSock2.h>
#include <windows.h>
#include <unknwn.h>
#include <string>

#include "Global.h"

namespace example
{
    class ClassFactory : public IClassFactory
    {
    public:
        // IUnknown
        IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();

        // IClassFactory
        IFACEMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv);
        IFACEMETHODIMP LockServer(BOOL fLock);

        explicit ClassFactory(FPCOMPCREATOR pFuncTemp);
        ~ClassFactory();

    private:
        FPCOMPCREATOR pCreator;
        long m_cRef;
    };
}
#endif