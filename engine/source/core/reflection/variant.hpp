#pragma once

#include "definitions_core.hpp"
#include "log/logger.hpp"
#include "foundation/type_traits.hpp"

namespace Engine
{
    class IVariantInst
    {
    public:
        virtual ~IVariantInst() = default;

        virtual void* GetPtr() = 0;
    };

    template <typename Type>
    class VariantInst : public IVariantInst
    {
    public:
        VariantInst() = delete;

        VariantInst(Type value) : Value(value) {}

        ~VariantInst() override = default;

        void* GetPtr() final
        {
            return const_cast<void*>(reinterpret_cast<const void*>(std::addressof(Value)));
        }

    private:
        Type Value;
    };

    enum class EVariantStore : uint8
    {
        Copy = 0
    };
    #define USE_VARIANT_COPY    int8
    #define VARIANT_COPY        (int8)1

    class Variant;

    template <typename T>
    concept ExceptVariant = !std::is_same<std::remove_cvref_t<T>, Variant>::value;

    class Variant
    {
        template <typename T>
        using RawType = typename std::remove_cvref_t<T>;

        template <typename T>
        using ValueType = typename std::remove_reference_t<T>;
    public:
        Variant() = default;

        template <ExceptVariant Type>
        Variant(Type& value)
            : Const(std::is_const_v<Type>)
            , Instance(new VariantInst<ValueType<Type>&>(value)) {}

        template <ExceptVariant Type>
        Variant(Type& value, USE_VARIANT_COPY)
            : Const(false)
            , Instance(new VariantInst<RawType<Type>>(value)) {}

        template <ExceptVariant Type>
        Variant(Type&& value) : Const(false), Instance(new VariantInst<RawType<Type>>(Forward<RawType<Type>>(value))) {}

        Variant(const Variant& other) = default;

        Variant(Variant&& other) noexcept
            : Const(other.Const)
        {
            Instance = other.Instance;
            other.Instance = nullptr;
        }

        ~Variant() = default;

        Variant& operator= (const Variant& other)
        {
            if (&other != this)
            {
                Instance = other.Instance;
            }
            return *this;
        }

        Variant& operator= (Variant&& other) noexcept
        {
            if (&other != this)
            {
                Instance = other.Instance;
                other.Instance = nullptr;
            }
            return *this;
        }

        bool IsValid() const { return Instance != nullptr; }

        template <typename Type>
        Type* GetPtr() const
        {
            return Instance ? static_cast<Type*>(Instance->GetPtr()) : nullptr;
        }

        template <typename Type>
        Type& GetValue() const
        {
            PL_ASSERT(IsValid());
            return *static_cast<Type*>(Instance->GetPtr());
        }
    private:
        bool Const{ false };

        SharedPtr<IVariantInst> Instance{ nullptr };
    };
}