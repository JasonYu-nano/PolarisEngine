#pragma once

#include <type_traits>
#include "global.hpp"

using namespace Engine;

template <typename Type>
NODISCARD constexpr Type&& Forward(std::remove_reference_t<Type>& arg) noexcept
{
    return static_cast<Type&&>(arg);
}

template <typename Type>
NODISCARD constexpr inline Type&& Forward(std::remove_reference_t<Type>&& arg) noexcept
{
    static_assert(!std::is_lvalue_reference_v<Type>, "bad forward call");
    return static_cast<Type&&>(arg);
}

template <typename Type>
NODISCARD constexpr inline std::remove_reference_t<Type>&& MoveTemp(Type&& arg) noexcept
{
    static_assert(!std::is_rvalue_reference_v<Type> && std::is_reference_v<Type>, "argument must be left value");
    return static_cast<std::remove_reference_t<Type>&&>(arg);
}

template <typename From, typename To>
struct CopyQualifiersFrom { using Type = To; };

template <typename From, typename To>
struct CopyQualifiersFrom<const From, To> { using Type = const To; };

template <typename From, typename To>
struct CopyQualifiersFrom<volatile From, To> { using Type = volatile To; };

template <typename From, typename To>
struct CopyQualifiersFrom<const volatile From, To> { using Type = const volatile To; };

template <typename From, typename To>
struct CopyQualifiersAndRefFrom { using Type = typename CopyQualifiersFrom<From, To>::Type; };

template <typename From, typename To>
struct CopyQualifiersAndRefFrom<From, To&> { using Type = typename CopyQualifiersFrom<From, To>::Type&; };

template <typename From, typename To>
struct CopyQualifiersAndRefFrom<From, To&&> { using Type = typename CopyQualifiersFrom<From, To>::Type&&; };

template <typename From, typename To>
struct CopyQualifiersAndRefFrom<From&, To> { using Type = typename CopyQualifiersFrom<From, To>::Type&; };

template <typename From, typename To>
struct CopyQualifiersAndRefFrom<From&, To&> { using Type = typename CopyQualifiersFrom<From, To>::Type&; };

template <typename From, typename To>
struct CopyQualifiersAndRefFrom<From&, To&&> { using Type = typename CopyQualifiersFrom<From, To>::Type&; };

template <typename From, typename To>
struct CopyQualifiersAndRefFrom<From&&, To> { using Type = typename CopyQualifiersFrom<From, To>::Type&&; };

template <typename From, typename To>
struct CopyQualifiersAndRefFrom<From&&, To&> { using Type = typename CopyQualifiersFrom<From, To>::Type&; };

template <typename From, typename To>
struct CopyQualifiersAndRefFrom<From&&, To&&> { using Type = typename CopyQualifiersFrom<From, To>::Type&&; };

template <typename Type>
constexpr bool HasTrivialDestructorV = __has_trivial_destructor(Type);

template <typename Type>
constexpr bool HasUserDestructorV = __has_virtual_destructor(Type) || __has_user_destructor(Type);

/** return type depend predicate */
template <bool Predicate, typename TrueType, typename FalseType>
struct SwitchType;

template <typename TrueType, typename FalseType>
struct SwitchType<true, TrueType, FalseType>
{
    typedef TrueType Value;
};

template <typename TrueType, typename FalseType>
struct SwitchType<false, TrueType, FalseType>
{
    typedef FalseType Value;
};

template <typename T>
concept SignedIntegralType = std::is_signed_v<T> && std::is_integral_v<T>;

template <typename T>
concept UnsignedIntegralType = std::is_unsigned_v<T> && std::is_integral_v<T>;

template <typename T>
concept IntegralType = std::is_integral_v<T>;
