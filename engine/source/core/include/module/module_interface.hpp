#pragma once

#include "definitions_core.hpp"

namespace Engine
{
    class CORE_API IModuleInterface
    {
    public:
        virtual ~IModuleInterface() = default;

        virtual void Startup() = 0;

        virtual void Shutdown() = 0;
    };
}