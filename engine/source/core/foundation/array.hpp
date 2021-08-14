#pragma once

#include <vector>
#include <span>

template<typename Type>
using Vector = std::vector<Type>;

template<typename T, size_t MaxSize>
using Array = std::array<T, MaxSize>;

template<typename T>
using Span = std::span<T>;
