#pragma once
#include "definitions_core.hpp"
#include "predefine/platform.hpp"
#include "foundation/type_traits.hpp"
#include "memory/memory.hpp"
#include "log/logger.hpp"
#include "math/limit.hpp"

namespace Engine
{
    template<typename UIntType, EnableIfT<IsUnsignedIntegral<typename UIntType>::value, bool> = true>
    class HeapAllocator
    {
    public:
        typedef UIntType SizeType;

        HeapAllocator() = default;

        HeapAllocator(const HeapAllocator& other) = delete;

        HeapAllocator(HeapAllocator&& other)
        {
            Data = other.Data;
            other.Data = nullptr;
        }

        ~HeapAllocator()
        {
            if (Data)
            {
                Memory::Free(Data);
                Data = nullptr;
            }
        }

        HeapAllocator& operator= (HeapAllocator&& other)
        {
            Data = other.Data;
            other.Data = nullptr;
            return *this;
        }

        SizeType GetDefaultCapacity() const
        {
            return 0;
        }

        byte* GetAllocation() const
        {
            return Data;
        }

        SizeType CaculateValidCapacity(SizeType elementNum, SizeType oldCapacity, size_t elementSize) const
        {
            const size_t firstGrow = 4;
            const size_t constantGrow = 16;

            SizeType ret;
            PL_ASSERT(elementNum > oldCapacity && elementNum > 0);

            size_t grow = firstGrow;

            if (oldCapacity)
            {
                grow = size_t(elementNum) + 3 * size_t(elementNum) / 8 + constantGrow;
            }
            else if (size_t(elementNum) > grow)
            {
                grow = size_t(elementNum);
            }

            ret = (SizeType)grow;

            if (elementNum > ret)
            {
                ret = NumericLimits<SizeType>::Max();
            }

            return ret;
        }

        void Resize(SizeType elementNum, size_t elementSize)
        {
            if (Data != nullptr || elementNum > 0)
            {
                if (Data == nullptr)
                {
                    Data = (byte*)Memory::Malloc(elementNum * elementSize);
                }
                else
                {
                    Data = (byte*)Memory::Realloc(Data, elementNum * elementSize);
                }
            }
        }

    private:
        byte* Data{ nullptr };
    };
}