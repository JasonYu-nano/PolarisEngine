#pragma once
#include <type_traits>
#include "global.hpp"
//#include "foundation/uchar.hpp"

using namespace Engine;

template <typename Type>
NODISCARD constexpr Type&& Forward(std::remove_reference_t<Type>& arg) noexcept
{
    return static_cast<Type&&>(arg);
}

template <typename Type>
NODISCARD constexpr Type&& Forward(std::remove_reference_t<Type>&& arg) noexcept
{
    static_assert(!std::is_lvalue_reference_v<Type>, "bad forward call");
    return static_cast<Type&&>(arg);
}

template <typename Type>
NODISCARD constexpr std::remove_reference_t<Type>&& MoveTemp(Type&& arg) noexcept
{
    static_assert(!std::is_rvalue_reference_v<Type> && std::is_reference_v<Type>, "argument must be left value");
    return static_cast<std::remove_reference_t<Type>&&>(arg);
}

template<bool Test, typename Type = void>
using EnableIf = typename std::enable_if<Test, Type>;

template<bool Test, typename Type = void>
using EnableIfT = typename std::enable_if<Test, Type>::type;

template <typename Type>
using IsPointer = std::is_pointer<Type>;

template <typename Type>
constexpr bool IsUnsignedIntegralV = std::_Is_any_of_v<std::remove_cv_t<Type>, uint8, uint16, uint32, uint64>;

template <typename Type>
constexpr bool IsSignedIntegralV = std::_Is_any_of_v<std::remove_cv_t<Type>, int8, int16, int32, int64>;

template <typename Type>
constexpr bool IsIntegralV = std::_Is_any_of_v<std::remove_cv_t<Type>, int8, uint8, int16, uint16, int32, uint32, int64, uint64>;

template <typename Type>
struct IsUnsignedIntegral : std::bool_constant<IsUnsignedIntegralV<Type>> {};

template <typename Type>
struct IsIntegral : std::bool_constant<IsIntegralV<Type>> {};

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
concept SignedIntegralType = IsSignedIntegralV<T>;

template <typename T>
concept IntegralType = IsIntegralV<T>;
