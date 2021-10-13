#pragma once

#include <set>
#include <map>
#include <unordered_map>

template<typename Key, typename Value>
using Map = std::map<Key, Value>;

template<typename Key, typename Value>
using HashMap = std::unordered_map<Key, Value>;

template<typename Type>
using Set = std::set<Type>;