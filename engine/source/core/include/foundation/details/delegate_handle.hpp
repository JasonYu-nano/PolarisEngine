#pragma once

#include <atomic>
#include "global.hpp"
#include "definitions_core.hpp"

namespace Engine
{
    class DelegateHandle
    {
    public:
        DelegateHandle() = default;

        CORE_API static DelegateHandle GetNext()
        {
            DelegateHandle handle(NextID);
            ++NextID;
            return handle;
        }

        bool IsValid() const { return ID != INVALID_HANDLE_ID; }

        friend bool operator== (const DelegateHandle& lhs, const DelegateHandle& rhs)
        {
            return lhs.ID == rhs.ID;
        }

        friend bool operator!= (const DelegateHandle& lhs, const DelegateHandle& rhs)
        {
            return lhs.ID != rhs.ID;
        }
    private:
        explicit DelegateHandle(uint64 id) : ID(id) {}

        constexpr static uint64 INVALID_HANDLE_ID = 0;

        static std::atomic<uint64> NextID;

        uint64 ID{ INVALID_HANDLE_ID };
    };
}