#pragma once

#include "misc/type_hash.hpp"
#include "foundation/sparse_array.hpp"

namespace Engine
{
    template <typename ContainerType>
    class ConstSetIterator
    {
        using SparseIteratorType = typename ContainerType::SparseArrayType::ConstIterator;
        using ValueType = typename ContainerType::ValueType;
    public:
        explicit ConstSetIterator(const SparseIteratorType& iter)
            : It(iter)
        {}

        const ValueType& operator*() const { return (*It).MyVal; }

        const ValueType* operator->() const { return It->MyVal; }

        explicit operator bool() const
        {
            return (bool)It;
        }

        ConstSetIterator& operator++ ()
        {
            ++It;
            return *this;
        }

        friend bool operator== (const ConstSetIterator& lhs, const ConstSetIterator& rhs)
        {
            return lhs.It == rhs.It;
        }

        friend bool operator!= (const ConstSetIterator& lhs, const ConstSetIterator& rhs)
        {
            return !(lhs == rhs);
        }

    protected:
        SparseIteratorType It;
    };

    template <typename ContainerType>
    class SetIterator : public ConstSetIterator<ContainerType>
    {
        using Super = ConstSetIterator<ContainerType>;
        using SparseIteratorType = typename ContainerType::SparseArrayType::ConstIterator;
        using ValueType = typename ContainerType::ValueType;
    public:
        explicit SetIterator(const SparseIteratorType& iter)
            : Super(iter)
        {}

        ValueType& operator*() const { return const_cast<ValueType&>(Super::operator *()); }

        ValueType* operator->() const { return const_cast<ValueType*>(Super::operator ->()); }

        SetIterator& operator++ ()
        {
            Super::operator++();
            return *this;
        }

        friend bool operator== (const SetIterator& lhs, const SetIterator& rhs)
        {
            return lhs.It == rhs.It;
        }

        friend bool operator!= (const SetIterator& lhs, const SetIterator& rhs)
        {
            return !(lhs == rhs);
        }
    };

    /**
     * Determine default hash function and equals function of set key
     * @tparam Key
     */
    template <typename Key>
    struct DefaultSetKeyFunc
    {
        using KeyType = Key;

        static uint32 GetHashCode(const Key& key)
        {
            return Engine::GetHashCode(key);
        }

        static const Key& GetKey(const Key& element)
        {
            return element;
        }

        static bool Equals(const Key& lKey, const Key& rKey)
        {
            return lKey == rKey;
        }
    };

    template <typename SizeType>
    struct SetElemIndex
    {
        SetElemIndex() = default;

        SetElemIndex(SizeType index) : Index(index) {}

        bool IsValid() const { return Index != INDEX_NONE; }

        operator SizeType() const
        {
            return Index;
        }

        // Index in sparse array
        SizeType Index{ INDEX_NONE };
    };

    template <typename ValueType, typename SizeType>
    class SetHashBucketVal
    {
    public:
        SizeType Size{ 0 };
        ValueType* Data{ nullptr };
    };

    template <typename Elem, typename Alloc>
    class SetHashBucket
    {
    public:
        using AllocatorType = Alloc::template ElementAllocator<Elem>;
        using ValueType = typename AllocatorType::ValueType;
        using SizeType = typename AllocatorType::SizeType;

        SetHashBucket& operator= (const SetHashBucket& other)
        {
            ENSURE(this != &other);
            GetAlloc() = other.GetAlloc();
            CopyAssign(other);

            return *this;
        }

        SetHashBucket& operator= (SetHashBucket&& other)
        {
            ENSURE(this != &other);
            GetAlloc() = std::move(other.GetAlloc());
            MoveAssign(std::forward<SetHashBucket>(other));

            return *this;
        }

        SizeType Size() const
        {
            return Pair.SecondVal.Size;
        }

        ValueType* Data()
        {
            return Pair.SecondVal.Data;
        }

        const ValueType* Data() const
        {
            return Pair.SecondVal.Data;
        }

        constexpr SizeType MaxSize() const
        {
            return NumericLimits<SizeType>::Max();
        }

        void Resize(SizeType newSize)
        {
            auto& alloc = GetAlloc();
            auto& myVal = Pair.SecondVal;
            ValueType* newData = alloc.Allocate(newSize);
            ConstructElements(newData, newSize);
            DestructElements(myVal.Data, myVal.Size);

            myVal.Data = newData;
            myVal.Size = newSize;
        }

        bool CalculateGrowth(const SizeType newSize, SizeType& outSize) const
        {
            outSize = 1;
            if (newSize >= MIN_HASHED_ELEMS_COUNT)
            {
                outSize = Math::Min(Math::RoundUpToPowerOfTwo(newSize / ELEMS_PER_BUCKET + MIN_BUCKETS), MaxSize());
            }

            return Pair.SecondVal.Size != outSize;
        }

        /** Contains the head of SetElement list */
        ValueType& GetFirstIndex(uint32 hashCode) const
        {
            auto& myVal = Pair.SecondVal;
            return ((ValueType*)myVal.Data)[GetHashIndex(hashCode)];
        }

        SizeType GetHashIndex(uint32 hashCode) const
        {
            // Quick mod hashCode % BucketCount, BucketCount must be 2 ^ n
            return hashCode & (Pair.SecondVal.Size - 1);
        }

    private:
        AllocatorType& GetAlloc() { return Pair.GetFirst(); }

        const AllocatorType& GetAlloc() const { return Pair.GetFirst(); }

        void CopyAssign(const SetHashBucket& other)
        {
            auto& otherVal = other.Pair.SecondVal;
            auto& myVal = Pair.SecondVal;

            Resize(otherVal.Size);
            if (otherVal.Data)
            {
                ConstructElements(myVal.Data, otherVal.Data, myVal.Size);
            }
        }

        void MoveAssign(SetHashBucket&& other)
        {
            auto& myVal = Pair.SecondVal;
            auto& otherVal = other.Pair.SecondVal;

            if (myVal.Data)
            {
                DestructElements(myVal.Data, myVal.Size);
                auto& alloc = GetAlloc();
                alloc.Deallocate(myVal.Data, myVal.Size);
            }

            myVal.Size = otherVal.Size;
            myVal.Data = otherVal.Data;

            otherVal.Data = nullptr;
            otherVal.Size = 0;
        }

    private:
        static constexpr SizeType ELEMS_PER_BUCKET = 2;
        static constexpr SizeType MIN_BUCKETS = 8;
        static constexpr SizeType MIN_HASHED_ELEMS_COUNT = 4;

        CompressedPair<AllocatorType, SetHashBucketVal<ValueType, SizeType>> Pair;
    };

    template <typename Elem, typename KeyFun = DefaultSetKeyFunc<Elem>, typename Alloc = StandardAllocator<int32>>
    class Set
    {
        template <typename T, typename U, typename V, typename W> friend class Map;
    public:
        using ValueType = Elem;
        using KeyType = typename KeyFun::KeyType;
        using SizeType = Alloc::SizeType;
        using SetElemIndex = SetElemIndex<SizeType>;

        struct SetElement
        {
            explicit SetElement(const ValueType& val) : MyVal(val) {}

            explicit SetElement(ValueType&& val) : MyVal(std::forward<ValueType>(val)) {}

            ~SetElement() = default;

            ValueType MyVal;
            uint32 HashIndex = 0;
            SetElemIndex HashNextId;
        };

        using HashBucketType = SetHashBucket<SetElemIndex, Alloc>;
        using SparseArrayType = SparseArray<SetElement, Alloc>;
        using AllocatorType = SparseArrayType::AllocatorType;
        using BucketAllocatorType = typename Alloc::template ElementAllocator<SetElemIndex>;
        using Iterator = SetIterator<Set>;
        using ConstIterator = ConstSetIterator<Set>;

        explicit Set(const AllocatorType& alloc = AllocatorType())
            : Elements(alloc)
        {}

        Set(SizeType capacity, const AllocatorType& alloc = AllocatorType())
            : Elements(alloc)
        {
            Reserve(capacity);
        }

        Set(std::initializer_list<ValueType> initializer)
        {
            Append(initializer);
        }

        template <typename IteratorType>
        Set(IteratorType begin, IteratorType end)
        {
            do
            {
                Add(*begin);
                ++begin;
            }
            while (begin != end);
        }

        Set(const Set& other)
        {
            CopyAssign(other);
        }

        Set(Set&& other) noexcept
        {
            MoveAssign(std::forward<Set>(other));
        }

        Set& operator= (std::initializer_list<ValueType> initializer)
        {
            Append(initializer);
            return *this;
        }

        Set& operator= (const Set& other)
        {
            CopyAssign(other);
            return *this;
        }

        Set& operator= (Set&& other) noexcept
        {
            MoveAssign(std::forward<Set>(other));
            return *this;
        }

        /**
         * Adds the specified element to Set.
         * @param elem
         */
        ValueType& Add(const ValueType& elem)
        {
            return Emplace(elem);
        }

        /**
         * Moves the specified element to Set.
         * @param elem
         */
        ValueType& Add(ValueType&& elem)
        {
            return Emplace(std::forward<ValueType>(elem));
        }

        void Append(std::initializer_list<ValueType> initializer)
        {
            Reserve(Elements.Size() + static_cast<SizeType>(initializer.size()));
            for (const ValueType& element : initializer)
            {
                Emplace(element);
            }
        }

        /**
         * Determines whether a Set contains the specified element.
         * @param key
         * @return True if contains the specified element, otherwise false.
         */
        bool Contains(const KeyType& key) const
        {
            return FindIndex(key).IsValid();
        }

        /**
         * Finds an element with the given key in the set.
         * @param key
         * @return A pointer to an element with the given key.  If no element in the set has the given key, this will return nullptr.
         */
        ValueType* Find(const KeyType& key) const
        {
            ValueType* ret = nullptr;
            if (Size() > 0)
            {
                SetElemIndex* elementIndex = &GetFirstIndex(KeyFun::GetHashCode(key));
                while (elementIndex->IsValid())
                {
                    auto&& setElement = Elements[elementIndex->Index];
                    if (KeyFun::Equals(KeyFun::GetKey(setElement.MyVal), key))
                    {
                        ret = const_cast<ValueType*>(&setElement.MyVal);
                        break;
                    }
                    else
                    {
                        elementIndex = const_cast<SetElemIndex*>(&setElement.HashNextId);
                    }
                }
            }
            return ret;
        }

        /**
         * Removes the specified element from a Set.
         * @param key
         * @return True if remove success.
         */
        bool Remove(const KeyType& key)
        {
            bool ret = false;
            if (Size() > 0)
            {
                SetElemIndex* elementIndex = &GetFirstIndex(KeyFun::GetHashCode(key));
                while (elementIndex->IsValid())
                {
                    auto&& setElement = Elements[elementIndex->Index];
                    if (KeyFun::Equals(KeyFun::GetKey(setElement.MyVal), key))
                    {
                        uint32 pendingRemoveIndex = elementIndex->Index;
                        elementIndex->Index = setElement.HashNextId.Index;
                        Elements.RemoveAt(pendingRemoveIndex);
                        ret = true;
                        break;
                    }
                    else
                    {
                        elementIndex = &setElement.HashNextId;
                    }
                }
            }
            return ret;
        }

        /**
         * Empties the set.
         * @param slack The expected capacity after clear operation
         */
        void Clear(SizeType slack = 0)
        {
            ENSURE(slack >= 0);
            HashBucket.Resize(slack);
            Elements.Clear(slack);
        }

        /**
         * @return Number of elements in set.
         */
        SizeType Size() const
        {
            return Elements.Size();
        }

        /**
         * @return True if the set is empty and contains no elements.
         */
        bool Empty() const
        {
            return Size() == 0;
        }

        /**
         * Preallocates enough memory.
         * @param capacity
         */
        void Reserve(SizeType capacity)
        {
            if (capacity > Elements.Size())
            {
                Elements.Reserve(capacity);

                SizeType newBucketSize;
                if (HashBucket.CalculateGrowth(capacity, newBucketSize))
                {
                    Rehash(newBucketSize);
                }
            }
        }

        /**
        * Resizes set to given number of elements.
        * @param newSize
        */
        void Resize(SizeType newSize)
        {
            ENSURE(newSize >= 0);
            Elements.Resize(newSize);

            SizeType newBucketSize;
            if (HashBucket.CalculateGrowth(newSize), newBucketSize)
            {
                Rehash(newBucketSize);
            }
        }

        Iterator begin() { return Iterator(Elements.begin()); }

        ConstIterator begin() const { return ConstIterator(Elements.begin()); }

        Iterator end() { return Iterator(Elements.end()); }

        ConstIterator end() const { return ConstIterator(Elements.end()); }

    private:
        template <typename ElemType>
        ValueType& Emplace(ElemType&& val)
        {
            uint32 hashCode = KeyFun::GetHashCode(KeyFun::GetKey(val));

            SetElemIndex index = FindIndex(KeyFun::GetKey(val), hashCode);
            if (index.IsValid())
            {
                auto&& setElement = Elements[index.Index];
                std::destroy_at(&setElement.MyVal);
                setElement.MyVal = std::forward<ElemType>(val);
                return setElement.MyVal;
            }

            CheckRehash(Elements.Size() + 1);
            SizeType indexInSparseArray = Elements.AddUnconstructElement();
            SetElement* item = new(Elements.Data() + indexInSparseArray) SetElement(std::forward<ElemType>(val));
            LinkElement(SetElemIndex(indexInSparseArray), *item, hashCode);
            return item->MyVal;
        }

        /** Contains key index in sparse array */
        SetElemIndex FindIndex(const KeyType& key) const
        {
            return FindIndex(key, KeyFun::GetHashCode(key));
        }

        /** Contains key index in sparse array */
        SetElemIndex FindIndex(const KeyType& key, uint32 hashCode) const
        {
            if (Elements.Size() > 0)
            {
                for (SetElemIndex index = GetFirstIndex(hashCode); index.IsValid(); index = Elements[index].HashNextId)
                {
                    if (KeyFun::Equals(KeyFun::GetKey(Elements[index].MyVal), key))
                    {
                        // Return the first match, regardless of whether the set has multiple matches for the key or not.
                        return index;
                    }
                }
            }
            return SetElemIndex{};
        }

        /** Contains the head of SetElement list */
        SetElemIndex& GetFirstIndex(uint32 hashCode) const
        {
            ENSURE(Size() > 0);
            return HashBucket.GetFirstIndex(hashCode);
        }

        bool CheckRehash(SizeType size)
        {
            SizeType newSize = 0;
            if (HashBucket.CalculateGrowth(size, newSize))
            {
                Rehash(newSize);
                return true;
            }
            return false;
        }

        void Rehash(SizeType newSize)
        {
            // Free the old hash.
            HashBucket.Resize(newSize);

            // Add the existing elements to the new hash.
            for (typename SparseArrayType::Iterator iter = Elements.begin(); iter != Elements.end(); ++iter)
            {
                LinkElement(SetElemIndex(iter.GetIndex()), *iter, KeyFun::GetHashCode(KeyFun::GetKey((*iter).MyVal)));
            }
        }

        void LinkElement(SetElemIndex index, SetElement& elem, uint32 hashCode) const
        {
            // get the index of hash bucket.
            elem.HashIndex = HashBucket.GetHashIndex(hashCode);

            // Link the element into the hash bucket.
            elem.HashNextId = GetFirstIndex(hashCode);
            GetFirstIndex(hashCode) = index;
        }

        void CopyAssign(const Set& other)
        {
            Elements = other.Elements;
            HashBucket = other.HashBucket;
        }

        void MoveAssign(Set&& other)
        {
            HashBucket = std::move(other.HashBucket);
            Elements = std::move(other.Elements);
        }

    private:
        SparseArrayType Elements;
        HashBucketType HashBucket;
    };
}

template <typename KeyType>
void* operator new(size_t size, const typename Engine::Set<KeyType>::SetElement& element)
{
    return &element.MyVal;
}