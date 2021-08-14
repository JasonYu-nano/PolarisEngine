#pragma once
#include <type_traits>
#include "predefine/platform.hpp"

template <typename Type>
NODISCARD constexpr Type&& Forward(std::remove_reference_t<Type>& arg) noexcept 
{
    return static_cast<Type&&>(arg);
}

template <class Type>
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
