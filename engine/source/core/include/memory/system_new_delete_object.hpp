#pragma once

#include <new>
#include "definitions_core.hpp"

namespace Engine
{
    /** inherit from SystemNewDeleteObject to avoid use overriden operator new delete */
    class CORE_API SystemNewDeleteObject
    {
    public:
        void* operator new(size_t size) { return std::malloc(size); }

        void* operator new[](size_t size) { return std::malloc(size); }

        void operator delete(void* ptr) { std::free(ptr); }

        void operator delete[](void* ptr) {std::free(ptr); }
    };
}
