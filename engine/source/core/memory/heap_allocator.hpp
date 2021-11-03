#pragma once
#include "definitions_core.hpp"
#include "predefine/platform.hpp"
#include "foundation/type_traits.hpp"
#include "memory/memory.hpp"

namespace Engine
{
    template<typename UIntType, EnableIfT<IsUnsignedIntegral<typename UIntType>::value, bool> = true>
    class CORE_API HeapAllocator
    {
        typedef UIntType SizeType;
    public:
        ~HeapAllocator()
        {
            if (Data)
            {
                Memory::Free(Data);
                Data = nullptr;
            }
        }

        SizeType GetDefaultCapacity() const
        {
            return 0;
        }

        byte* GetAllocation()
        {
            return Data;
        }

        void Resize(SizeType elementNum, size_t elementSize)
        {
            if (Data != nullptr || elementNum > 0)
            {
                Data = (byte*)Memory::Realloc(Data, elementNum * elementSize);
            }
        }

    private:
        byte* Data{ nullptr };
    };
}