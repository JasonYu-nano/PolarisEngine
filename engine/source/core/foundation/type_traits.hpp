#pragma once
#include <type_traits>
#include "predefine/platform.hpp"

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
using IsIntegral = std::is_integral<Type>;

template <typename Type>
using IsPointer = std::is_pointer<Type>;

template <typename Type>
constexpr bool IsUnsignedIntegralV = std::_Is_any_of_v<std::remove_cv_t<Type>, uint8, uint16, uint32, uint64>;

template <typename Type>
struct IsUnsignedIntegral : std::bool_constant<IsUnsignedIntegralV<Type>> {};

template <typename Type>
constexpr bool HasTrivialDestructorV = __has_trivial_destructor(Type);

template <typename Type>
constexpr bool HasUserDestructorV = __has_user_destructor(Type) || __has_virtual_destructor(Type);