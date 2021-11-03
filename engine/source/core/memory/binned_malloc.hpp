#pragma once

#include "memory/malloc_interface.hpp"
#include "memory/system_new_delete_object.hpp"

namespace Engine
{
    class CORE_API BinnedMalloc final : public IMalloc, public SystemNewDeleteObject
    {
    public:
        BinnedMalloc() = default;

        virtual ~BinnedMalloc() = default;

        virtual void* Malloc(size_t size, uint32 alignment) final;

        virtual void Free(void* ptr) final;

        virtual void* Realloc(void* ptr, size_t size, uint32 alignment) final;

        virtual void SetupCurrentThreadTLS() final;

    private:
    };
}