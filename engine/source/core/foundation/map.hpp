#pragma once

#include <set>
#include <map>
#include <unordered_map>
#include <tuple>
#include "definitions_core.hpp"
#include "foundation/set.hpp"

namespace Engine
{
    template <typename KeyType, typename ValueType>
    class Pair
    {
    public:
        Pair() = default;

        Pair(KeyType key, ValueType value)
            : Key(key)
            , Value(value)
        {}

        explicit Pair(const std::tuple<KeyType, ValueType>& pair)
            : Key(std::get<0>(pair))
            , Value(std::get<1>(pair))
        {}

        template<typename InKeyType, typename InValueType>
        explicit operator std::tuple<KeyType, ValueType>()
        {
            return std::make_tuple(static_cast<InKeyType>(Key), static_cast<InValueType>(Value));
        }

    public:
        KeyType Key;
        ValueType Value;
    };

    template <typename KeyType, typename ValueType>
    struct MapDefaultHashFunc
    {
        static uint32 GetHashCode(const KeyType& key)
        {
            return Engine::GetHashCode(key);
        }

        static const KeyType& GetKey(const Pair<KeyType, ValueType>& element)
        {
            return element.Key;
        }

        static bool Equals(const KeyType& lKey, const KeyType& rKey)
        {
            return lKey == rKey;
        }
    };

    template <typename KeyType, typename ValueType>
    class Map
    {
        using TPairContainer = Set<Pair<KeyType, ValueType>, MapDefaultHashFunc<KeyType, ValueType>>;
    public:
        Map() = default;

        Map(std::initializer_list<Pair<KeyType, ValueType>> initializer)
        {
            for (const auto& pair : initializer)
            {
                Add(pair.Key, pair.Value);
            }
        }

        ValueType& Add(const KeyType& key, const ValueType& value)
        {
            return Emplace(key, value);
        }

        ValueType& Add(const KeyType& key, ValueType&& value)
        {
            return Emplace(key, value);
        }

        ValueType& Add(KeyType&& key, const ValueType& value)
        {
            return Emplace(key, value);
        }

        ValueType& Add(KeyType&& key, ValueType&& value)
        {
            return Emplace(key, value);
        }

        ValueType& FindOrAdd(const KeyType& key, const ValueType& value)
        {
            return FindOrAddImpl(key, value);
        }

        ValueType& FindOrAdd(const KeyType& key, ValueType&& value)
        {
            return FindOrAddImpl(key, value);
        }

        ValueType& FindOrAdd(KeyType&& key, const ValueType& value)
        {
            return FindOrAddImpl(key, value);
        }

        ValueType& FindOrAdd(KeyType&& key, ValueType&& value)
        {
            return FindOrAddImpl(key, value);
        }

        ValueType* Find(const KeyType& key)
        {
            if (Pair<KeyType, ValueType>* pair = Pairs.Find(key))
            {
                return &pair->Value;
            }
            return nullptr;
        }

        bool Contains(const KeyType& key)
        {
            return Pairs.Contains(key);
        }

        bool Remove(const KeyType& key)
        {
            return Pairs.Remove(key);
        }

    protected:
        template <typename InKeyType, typename InValueType>
        ValueType& Emplace(InKeyType&& key, InValueType&& value)
        {
            return Pairs.Emplace(Pair(Forward<InKeyType>(key), Forward<InValueType>(value))).Value;
        }

        //TODO: Calculate hash value twice here
        template <typename InKeyType, typename InValueType>
        InValueType& FindOrAddImpl(InKeyType&& key, InValueType&& value)
        {
            if (Pair<KeyType, ValueType>* pair = Pairs.Find(Forward<InKeyType>(key)))
            {
                return pair->Value;
            }
            else
            {
                return Emplace(Forward<InKeyType>(key), Forward<InValueType>(value));
            }
        }

    private:
        TPairContainer Pairs;
    };
}