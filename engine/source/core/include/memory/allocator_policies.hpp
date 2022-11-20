#pragma once

#include "global.hpp"
#include "foundation/type_traits.hpp"
#include "memory/memory.hpp"
#include "math/limit.hpp"

namespace Engine
{
    template <SignedIntegralType IntType>
    class StandardAllocator
    {
    public:

        template <typename ElementType>
        class ElementAllocator
        {
        public:
            using SizeType = IntType;
            using ValueType = ElementType;

            ElementAllocator() = default;

            ElementAllocator(const ElementAllocator& other) = delete;

            ElementAllocator(ElementAllocator&& other) noexcept = default;

            NODISCARD const ValueType* Allocate(SizeType n)
            {
                return static_cast<ValueType>(Memory::Malloc(n * sizeof(ElementType), alignof(ElementType)));
            }

            constexpr void Deallocate(ValueType* ptr, SizeType n)
            {
                Memory::Free(ptr);
            }
        };
    };

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

            ElementAllocator& operator= (ElementAllocator&& other) noexcept
            {
                Data = other.Data;
                other.Data = nullptr;
                return *this;
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

    template <uint32 InlineSize, typename SecondaryAllocator = DefaultAllocator>
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

            ElementAllocator& operator= (ElementAllocator&& other) noexcept
            {
                if (other.SecondaryData.Empty())
                {
                    Memory::Memcpy(InlineData, other.InlineData, sizeof(ElementType) * InlineSize);
                }

                SecondaryData = MoveTemp(other.SecondaryData);
                return *this;
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
                Memory::Memcpy(SecondaryData.GetAllocation(), InlineData, sizeof(ElementType) * InlineSize);
            }

        private:
            ElementType InlineData[InlineSize];

            SecondaryAllocatorType SecondaryData;
        };
    };

    template <uint32 Size>
    class FixedAllocator
    {
    public:

        template <typename ElementType>
        class ElementAllocator
        {
        public:
            using SizeType = uint8;

            ElementAllocator() = default;

            ElementAllocator(const ElementAllocator& other) = delete;

            ElementAllocator(ElementAllocator&& other) noexcept
            {
                Memory::Memcpy(Data, other.Data, sizeof(ElementType) * Size);
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
                ENSURE(size <= Size);
            }

        private:
            ElementType Data[Size];
        };
    };
}