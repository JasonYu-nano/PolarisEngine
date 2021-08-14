#pragma once

#include <map>
#include <hash_map>

template<typename Key, typename Value>
using Map = std::map<Key, Value>;

template<typename Key, typename Value>
using HashMap = std::hash_map<Key, Value>;