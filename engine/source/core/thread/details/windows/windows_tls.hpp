#pragma once

#include "predefine/platform.hpp"

namespace Engine
{
    class WindowsTLS
    {
    public:
        WindowsTLS() = delete;

        uint32 GetThreadId() const;

        /** get tls index, maybe invalid */
        uint32 AllocTls();

        bool IsTlsIndexValid(uint32 tlsIndex);

        void* GetTlsValue(uint32 tlsIndex);

        void SetTlsValue(uint32 tlsIndex, void* value);
    };

    typedef WindowsTLS PlatformTLS;
}