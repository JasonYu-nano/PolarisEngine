#pragma once

#include <tuple>
#include "definitions_core.hpp"
#include "foundation/set.hpp"
#include "tuple.hpp"

namespace Engine
{
    template <typename PairIterType, typename PairType>
    class ConstMapIterator
    {
    public:
        explicit ConstMapIterator(const PairIterType& pairIter)
            : PairIter(pairIter)
        {}

        const PairType& operator*() const { return *PairIter; }

        const PairType* operator->() const { return &(*PairIter); }

        explicit operator bool() const
        {
            return (bool)PairIter;
        }

        ConstMapIterator& operator++ ()
        {
            ++PairIter;
            return *this;
        }

        friend bool operator== (const ConstMapIterator& lhs, const ConstMapIterator& rhs)
        {
            return lhs.PairIter == rhs.PairIter;
        }

        friend bool operator!= (const ConstMapIterator& lhs, const ConstMapIterator& rhs)
        {
            return !(lhs == rhs);
        }

    protected:
        PairIterType PairIter;
    };

    template <typename PairIterType, typename PairType>
    class MapIterator : public ConstMapIterator<PairIterType, PairType>
    {
        using Super = ConstMapIterator<PairIterType, PairType>;
    public:
        explicit MapIterator(const PairIterType& pairIter) : Super(pairIter) {}

        PairType& operator*() const { return const_cast<PairType&>(*(this->PairIter)); }

        PairType* operator->() const { return const_cast<PairType*>(&(*(this->PairIter))); }

        MapIterator& operator++ ()
        {
            Super::operator++();
            return *this;
        }
    };

    template <typename Key, typename Value>
    struct MapDefaultHashFun
    {
        using KeyType = Key;
        using ValueType = Value;

        static uint32 GetHashCode(const KeyType& key)
        {
            return Engine::GetHashCode(key);
        }

        static const KeyType& GetKey(const Tuple<KeyType, ValueType>& element)
        {
            return element.Key;
        }

        static bool Equals(const KeyType& lKey, const KeyType& rKey)
        {
            return lKey == rKey;
        }
    };

    template <typename Key, typename Value, typename KeyFun = MapDefaultHashFun<Key, Value>, typename Alloc = StandardAllocator<int32>>
    class Map
    {
    public:
        using KeyType = Key;
        using ValueType = Value;
        using PairType = Tuple<KeyType, ValueType>;
        using AllocatorType = Alloc::template ElementAllocator<PairType>;
        using SizeType = typename AllocatorType::SizeType;
        using SetType = Set<PairType, KeyFun, Alloc>;
        using ConstIterator = ConstMapIterator<typename SetType::ConstIterator, PairType>;
        using Iterator = MapIterator<typename SetType::ConstIterator, PairType>;
    public:
        explicit Map() = default;

        Map(SizeType capacity)
            : Pairs(capacity)
        {}

        Map(std::initializer_list<PairType> initializer)
        {
            for (const auto& pair : initializer)
            {
                Add(pair.Key, pair.Value);
            }
        }

        Map(const Map& other) : Pairs(other.Pairs) {}

        Map(Map&& other) noexcept : Pairs(std::move(other.Pairs)) {}

        Map& operator= (std::initializer_list<PairType> initializer)
        {
            Pairs.Append(initializer);
            return *this;
        }

        Map& operator= (const Map& other)
        {
            ENSURE(this != &other);
            Pairs = other.Pairs;
            return *this;
        }

        Map& operator= (Map&& other) noexcept
        {
            ENSURE(this != &other);
            Pairs = std::move(other.Pairs);
            return *this;
        }

        ValueType& Add(const KeyType& key, const ValueType& value)
        {
            return Emplace(key, value);
        }

        ValueType& Add(const KeyType& key, ValueType&& value)
        {
            return Emplace(key, std::forward<ValueType>(value));
        }

        ValueType& Add(KeyType&& key, const ValueType& value)
        {
            return Emplace(std::forward<KeyType>(key), value);
        }

        ValueType& Add(KeyType&& key, ValueType&& value)
        {
            return Emplace(std::forward<KeyType>(key), std::forward<ValueType>(value));
        }

        ValueType& FindOrAdd(const KeyType& key, const ValueType& value)
        {
            return FindOrAddImpl(key, value);
        }

        ValueType& FindOrAdd(const KeyType& key, ValueType&& value)
        {
            return FindOrAddImpl(key, std::forward<ValueType>(value));
        }

        ValueType& FindOrAdd(KeyType&& key, const ValueType& value)
        {
            return FindOrAddImpl(std::forward<KeyType>(key), value);
        }

        ValueType& FindOrAdd(KeyType&& key, ValueType&& value)
        {
            return FindOrAddImpl(std::forward<KeyType>(key), std::forward<ValueType>(value));
        }

        ValueType* Find(const KeyType& key) const
        {
            if (PairType* pair = Pairs.Find(key))
            {
                return &pair->Value;
            }
            return nullptr;
        }

        ValueType& FindRef(const KeyType& key)
        {
            PairType* pair = Pairs.Find(key);
            ENSURE(pair);
            return pair->Value;
        }

        const ValueType& FindRef(const KeyType& key) const
        {
            PairType* pair = Pairs.Find(key);
            ENSURE(pair);
            return pair->Value;
        }

        bool Contains(const KeyType& key) const
        {
            return Pairs.Contains(key);
        }

        bool Remove(const KeyType& key)
        {
            return Pairs.Remove(key);
        }

        void Clear(SizeType slack = 0)
        {
            Pairs.Clear(slack);
        }

        SizeType Size() const
        {
            return Pairs.Size();
        }

        Iterator begin()
        {
            return Iterator(Pairs.begin());
        }

        ConstIterator begin() const
        {
            return ConstIterator(Pairs.begin());
        }

        Iterator end()
        {
            return Iterator(Pairs.end());
        }

        ConstIterator end() const
        {
            return ConstIterator(Pairs.begin());
        }

    private:
        template <typename AnyKeyType, typename AnyValueType>
        ValueType& Emplace(AnyKeyType&& key, AnyValueType&& value)
        {
            return Pairs.Emplace(PairType(std::forward<AnyKeyType>(key), std::forward<AnyValueType>(value))).Value;
        }

        template <typename AnyKeyType, typename AnyValueType>
        ValueType& FindOrAddImpl(AnyKeyType&& key, AnyValueType&& value)
        {
            if (PairType* pair = Pairs.Find(std::forward<AnyKeyType>(key)))
            {
                return pair->Value;
            }
            else
            {
                return Emplace(std::forward<AnyKeyType>(key), std::forward<AnyValueType>(value));
            }
        }

    private:
        SetType Pairs;
    };
}