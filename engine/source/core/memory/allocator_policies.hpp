#pragma once
#include "definitions_core.hpp"
#include "predefine/platform.hpp"
#include "foundation/type_traits.hpp"
#include "memory/memory.hpp"
#include "log/logger.hpp"
#include "math/limit.hpp"

namespace Engine
{
    template <SignedIntegralType IntType>
    class HeapSizeAllocator
    {
    public:

        template <typename ElementType>
        class ElementAllocator
        {
        public:
            using SizeType = IntType;

            ElementAllocator() = default;

            ElementAllocator(const ElementAllocator& other) = delete;

            ElementAllocator(ElementAllocator&& other) noexcept
            {
                Data = other.Data;
                other.Data = nullptr;
            }

            bool Empty() const
            {
                return Data == nullptr;
            }

            SizeType GetDefaultSize() const
            {
                return 0;
            }

            byte* GetAllocation() const
            {
                return Data;
            }

            void Resize(SizeType size)
            {
                if (Data != nullptr || size > 0)
                {
                    if (Data == nullptr)
                    {
                        Data = (byte*)Memory::Malloc(size * sizeof(ElementType));
                    }
                    else
                    {
                        Data = (byte*)Memory::Realloc(Data, size * sizeof(ElementType));
                    }
                }
            }

        private:
            byte* Data{ nullptr };
        };
    };

    using DefaultAllocator = HeapSizeAllocator<int32>;

    template<typename T>
    concept AvailableSecondaryAllocator = requires(T a)
    {
        T::ElementAllocator::Empty();
    };

    template <uint8 InlineSize, AvailableSecondaryAllocator SecondaryAllocator = DefaultAllocator>
    class InlineAllocator
    {
    public:

        template <typename ElementType>
        class ElementAllocator
        {
            using SecondaryAllocatorType = typename SecondaryAllocator::template ElementAllocator<ElementType>;
        public:
            using SizeType = int32;

            ElementAllocator() = default;

            ElementAllocator(const ElementAllocator& other) = delete;

            ElementAllocator(ElementAllocator&& other) noexcept
            {
                if (other.SecondaryData.Empty())
                {
                    Memory::Memmove(InlineData, other.InlineData, sizeof(ElementType) * InlineSize);
                }

                SecondaryData = MoveTemp(other.SecondaryData);
            }

            byte* GetAllocation() const
            {
                if (SecondaryData.Empty())
                {
                    return (byte*)InlineData;
                }
                else
                {
                    return SecondaryData.GetAllocation();
                }
            }

            SizeType GetDefaultSize() const
            {
                return InlineSize;
            }

            void Resize(int32 size)
            {
                if (!SecondaryData.Empty())
                {
                    SecondaryData.Resize(size);
                    return;
                }

                if (size <= InlineSize)
                {
                    return;
                }

                SecondaryData.Resize(size);
                Memory::Memmove(SecondaryData.GetAllocation(), InlineData, sizeof(ElementType) * InlineSize);
            }

        private:
            ElementType InlineData[InlineSize];

            SecondaryAllocatorType SecondaryData;
        };
    };

    template <uint8 Size>
    class FixedAllocatorV2
    {
    public:

        template <typename ElementType>
        class ElementAllocator
        {
            using SizeType = uint8;

            SizeType GetDefaultCapacity() const
            {
                return Size;
            }

            byte* GetAllocation() const
            {
                return (byte*)Data;
            }

            SizeType GetDefaultSize() const
            {
                return Size;
            }

            void Resize(SizeType size)
            {
                PL_ASSERT(size <= Size);
            }

        private:
            ElementType Data[Size];
        };
    };

    template<IntegralType IntType>
    class HeapAllocator
    {
    public:
        typedef IntType SizeType;

        HeapAllocator() = default;

        HeapAllocator(const HeapAllocator& other) = delete;

        HeapAllocator(HeapAllocator&& other) noexcept
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

        HeapAllocator& operator= (HeapAllocator&& other) noexcept
        {
            Data = other.Data;
            other.Data = nullptr;
            return *this;
        }

        bool HasAllocation() const
        {
            return Data != nullptr;
        }

        SizeType GetDefaultCapacity() const
        {
            return 0;
        }

        byte* GetAllocation() const
        {
            return Data;
        }

        SizeType CalculateValidCapacity(SizeType elementNum, SizeType oldCapacity, size_t elementSize) const
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
                else if (elementNum == 0)
                {
                    Data = nullptr;
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

    template <typename ElementType, uint32 Size>
    class FixedAllocator
    {
    public:
        using SizeType = int32;

        FixedAllocator& operator= (FixedAllocator&& other) noexcept
        {
            Data = other.Data;
            other.Data = nullptr;
            return *this;
        }

        bool HasAllocation() const
        {
            return false;
        }

        SizeType GetDefaultCapacity() const
        {
            return Size;
        }

        byte* GetAllocation() const
        {
            return (byte*)Data;
        }

        SizeType CalculateValidCapacity(SizeType elementNum, SizeType oldCapacity, size_t elementSize) const
        {
            PL_ASSERT(elementNum <= Size);
            return Size;
        }

        void Resize(SizeType elementNum, size_t elementSize)
        {
            PL_ASSERT(elementNum <= Size);
            // do nothing
        }

    private:
        ElementType Data[Size];
    };

    template <typename ElementType, uint32 InlineSize, typename SecondaryAllocator, typename InSizeType>
    class InlineAllocator
    {
    public:
        using SizeType = InSizeType;

        InlineAllocator& operator= (InlineAllocator&& other) noexcept
        {
            if (!other.HasAllocation())
            {
                Memory::Memcpy(Data, other.Data, sizeof(ElementType) * InlineSize);
                Allocator.Resize(0, sizeof(ElementType));
            }
            else
            {
                Allocator = MoveTemp(other.Allocator);
            }

            return *this;
        }

        bool HasAllocation() const
        {
            return Allocator.HasAllocation();
        }

        SizeType GetDefaultCapacity() const
        {
            return InlineSize;
        }

        byte* GetAllocation() const
        {
            if (Allocator.HasAllocation())
            {
                return Allocator.GetAllocation();
            }
            else
            {
                return reinterpret_cast<byte*>(const_cast<ElementType*>(Data));
            }
        }

        SizeType CalculateValidCapacity(SizeType elementNum, SizeType oldCapacity, size_t elementSize) const
        {
            return elementNum <= InlineSize ? InlineSize : Allocator.CalculateValidCapacity(elementNum, oldCapacity, elementSize);
        }

        void Resize(SizeType elementNum, size_t elementSize)
        {
            if (elementNum <= InlineSize)
            {
                if (Allocator.HasAllocation())
                {
                    Memory::Memcpy(Data, Allocator.GetAllocation(), elementNum * sizeof(ElementType));
                }
            }
            else
            {
                Allocator.Resize(elementNum, elementSize);
                Memory::Memcpy(Allocator.GetAllocation(), Data, InlineSize * sizeof(ElementType));
            }
        }

    private:
        ElementType Data[InlineSize];
        SecondaryAllocator Allocator;
    };
}