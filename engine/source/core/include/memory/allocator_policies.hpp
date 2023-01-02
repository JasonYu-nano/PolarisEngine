#pragma once

#include "global.hpp"
#include "foundation/type_traits.hpp"
#include "memory/memory.hpp"
#include "math/limit.hpp"
#include "memory/untyped_data.hpp"

namespace Engine
{
    template <SignedIntegralType IntType = int32>
    class StandardAllocator
    {
    public:
        using SizeType = IntType;

        template <typename ElementType>
        class ElementAllocator
        {
        public:
            using SizeType = IntType;
            using ValueType = ElementType;

            ElementAllocator() = default;

            ElementAllocator(const ElementAllocator& other) noexcept = default;

            ElementAllocator(ElementAllocator&& other) noexcept = default;

            ElementAllocator& operator=(const ElementAllocator& other) = default;

            NODISCARD ValueType* Allocate(SizeType n)
            {
                return static_cast<ValueType*>(Memory::Malloc(n * sizeof(ValueType), alignof(ValueType)));
            }

            constexpr void Deallocate(ValueType* ptr, SizeType n)
            {
                Memory::Free(ptr);
            }
        };
    };

    template <uint32 InlineSize, SignedIntegralType Type = int32>
    class InlineAllocator
    {
        static_assert(InlineSize > 0, "InlineSize must over 0");
    public:
        using SizeType = Type;

        template <typename Elem>
        class ElementAllocator
        {
        public:
            using SizeType = Type;
            using ValueType = Elem;

            ElementAllocator() = default;

            ElementAllocator(const ElementAllocator& other) noexcept = default;

            ElementAllocator(ElementAllocator&& other) noexcept = default;

            ElementAllocator& operator=(const ElementAllocator& other) = default;

            NODISCARD ValueType* Allocate(SizeType n)
            {
                if (n <= InlineSize)
                {
                    return (ValueType*)Buffer;
                }
                return static_cast<ValueType*>(Memory::Malloc(n * sizeof(ValueType), alignof(ValueType)));
            }

            constexpr void Deallocate(ValueType* ptr, SizeType n)
            {
                if (n > InlineSize)
                {
                    Memory::Free(ptr);
                }
            }

        private:
            UntypedData<ValueType> Buffer[InlineSize];
        };
    };
}