//#include "precompiled_core.hpp"
#include "thread/details/windows/windows_tls.hpp"
#include <windows.h>

namespace Engine
{
    uint32 WindowsTLS::GetThreadId() const
    {
        return ::GetCurrentThreadId();
    }

    uint32 WindowsTLS::AllocTls()
    {
        return ::TlsAlloc();
    }

    bool WindowsTLS::IsTlsIndexValid(uint32 tlsIndex)
    {
        return tlsIndex != TLS_OUT_OF_INDEXES;
    }

    void* WindowsTLS::GetTlsValue(uint32 tlsIndex)
    {
        return ::TlsGetValue(tlsIndex);
    }

    void WindowsTLS::SetTlsValue(uint32 tlsIndex, void* value)
    {
        ::TlsSetValue(tlsIndex, value);
    }
}