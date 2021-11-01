#pragma once

#include "definitions_core.hpp"
#include "predefine/platform.hpp"
#include "memory/malloc_interface.hpp"

namespace Engine
{
    class CORE_API WindowsMemory
    {
    public:
        static IMalloc* GetDefaultMalloc();

        static uint32 GetDefaultAlignment();
    private:
        static uint32 SDefaultAlignment;
    };

    typedef WindowsMemory PlatformMemory;
}