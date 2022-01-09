#pragma once

#include "misc/type_hash.hpp"
#include "foundation/sparse_array.hpp"

namespace Engine
{
#pragma region iterator
    template <typename ContainerType, typename KeyType>
    class ConstSetIterator
    {
        using ConstInnerIterator = typename ContainerType::TSparseArray::ConstIterator;
    public:
        explicit ConstSetIterator(const ConstInnerIterator& iter)
            : InnerIterator(iter)
        {}

        const KeyType& operator*() const { return (*InnerIterator).Element; }

        const KeyType* operator->() const { return InnerIterator->Element; }

        ConstSetIterator& operator++ ()
        {
            ++InnerIterator;
            return *this;
        }

        friend bool operator== (const ConstSetIterator& lhs, const ConstSetIterator& rhs)
        {
            return lhs.InnerIterator == rhs.InnerIterator;
        }

        friend bool operator!= (const ConstSetIterator& lhs, const ConstSetIterator& rhs)
        {
            return !(lhs == rhs);
        }

    protected:
        ConstInnerIterator InnerIterator;
    };

    template <typename ContainerType, typename KeyType>
    class SetIterator : public ConstSetIterator<ContainerType, KeyType>
    {
        using Super = ConstSetIterator<ContainerType, KeyType>;
        using ConstInnerIterator = typename ContainerType::TSparseArray::ConstIterator;
    public:
        explicit SetIterator(const ConstInnerIterator& iter)
            : Super(iter)
        {}

        KeyType& operator*() const { return const_cast<KeyType&>(Super::operator *()); }

        KeyType* operator->() const { return const_cast<KeyType*>(Super::operator ->()); }

        SetIterator& operator++ ()
        {
            Super::operator++();
            return *this;
        }

        friend bool operator== (const SetIterator& lhs, const SetIterator& rhs)
        {
            return lhs.InnerIterator == rhs.InnerIterator;
        }

        friend bool operator!= (const SetIterator& lhs, const SetIterator& rhs)
        {
            return !(lhs == rhs);
        }
    };

#pragma endregion iterator

    template <typename Type>
    struct DefaultHashFunc
    {
        static uint32 GetHashCode(const Type& value)
        {
            return Engine::GetHashCode(value);
        }
    };

    /**
     * Determine default hash function and equals function of set key
     * @tparam Key
     */
    template <typename Key>
    struct DefaultSetKeyFunc
    {
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

    /** Encapsulates the allocators used by a set in a single type. */
    template <
        typename InSparseArrayAllocator,
        typename InHashAllocator,
        uint32 ElementsPerHashBucket = 2,
        uint32 MinHashBuckets = 8,
        uint32 MinCountOfHashedElements = 4>
    struct SetAllocator
    {
        static uint32 GetNumberOfHashBuckets(uint32 hashedElementsCount)
        {
            if (hashedElementsCount >= MinCountOfHashedElements)
            {
                return Math::RoundUpToPowerOfTwo(hashedElementsCount / ElementsPerHashBucket + MinHashBuckets);
            }

            return 1;
        }

        typedef InSparseArrayAllocator SparseArrayAllocator;
        typedef InHashAllocator        HashAllocator;
    };

    /** index of set in sparse array */
    struct SetElementIndex
    {
        bool IsValid() const { return Index != INDEX_NONE; }

        operator int32() const
        {
            return Index;
        }

        // Index in sparse array
        int32 Index{ INDEX_NONE };
    };

    using DefaultSetAllocator = SetAllocator<HeapAllocator<uint32>, HeapAllocator<uint32>>;

    template <typename ElementType, typename KeyFunc = DefaultSetKeyFunc<ElementType>, typename SetAllocator = DefaultSetAllocator>
    class Set
    {
        struct SetElement
        {
            explicit SetElement(const ElementType& element)
            {
                Element = element;
            }

            explicit SetElement(ElementType&& element)
            {
                Element = element;
            }

            uint32 HashIndex = 0;
            SetElementIndex HashNextId;
            ElementType Element;
        };

        using TSparseArray = SparseArray<SetElement>;

        friend class ConstSetIterator<Set, ElementType>;
        friend class SetIterator<Set, ElementType>;
        template <typename K, typename V> friend class Map;

    public:
        using Iterator = SetIterator<Set, ElementType>;
        using ConstIterator = ConstSetIterator<Set, ElementType>;

    public:
        Set() = default;

        Set(std::initializer_list<ElementType> initializer)
        {
            Append(initializer);
        }

        /**
         * Adds the specified element to Set.
         * 
         * @param T element
         */
        void Add(const ElementType& element)
        {
            Emplace(element);
        }

        /**
         * Moves the specified element to Set.
         *
         * @param T element
         */
        void Add(ElementType&& element)
        {
            Emplace(element);
        }

        void Append(std::initializer_list<ElementType> initializer)
        {
            Reserve(Elements.GetCount() + (int32)initializer.size());
            for (auto&& element : initializer)
            {
                Add(element);
            }
        }

        /**
         * Determines whether a Set contains the specified element.
         * 
         * @param T key
         * @return boolean true if contains the specified element, otherwise false.
         */
        template <typename KeyType>
        bool Contains(const KeyType& key) const
        {
            return FindIndex(key).IsValid();
        }

        template <typename KeyType>
        ElementType* Find(const KeyType& key) const
        {
            ElementType* ret = nullptr;
            SetElementIndex* elementIndex = &GetFirstIndex(KeyFunc::GetHashCode(key));
            while (elementIndex->IsValid())
            {
                auto&& setElement = Elements[elementIndex->Index];
                if (KeyFunc::Equals(KeyFunc::GetKey(setElement.Element), key))
                {
                    ret = const_cast<ElementType*>(&setElement.Element);
                    break;
                }
                else
                {
                    elementIndex = const_cast<SetElementIndex*>(&setElement.HashNextId);
                }
            }
            return ret;
        }

        /**
         * Removes the specified element from a Set.
         * 
         * @param T key
         * @return boolean true if remove success.
         */
        template <typename KeyType>
        bool Remove(const KeyType& key)
        {
            SetElementIndex* elementIndex = &GetFirstIndex(KeyFunc::GetHashCode(key));
            bool ret = false;
            while (elementIndex->IsValid())
            {
                auto&& setElement = Elements[elementIndex->Index];
                if (KeyFunc::Equals(KeyFunc::GetKey(setElement.Element), key))
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
            return ret;
        }

        /**
         * Preallocates enough memory to contain Number elements.
         * 
         * @param count int32
         */
        void Reserve(int32 count)
        {
            if (count > static_cast<int32>(Elements.GetCount()))
            {
                Elements.Reserve(count);

                const uint32 newBucketCount = SetAllocator::GetNumberOfHashBuckets(static_cast<uint32>(count));
                if (!BucketCount || BucketCount < newBucketCount)
                {
                    BucketCount = newBucketCount;
                    Rehash();
                }
            }
        }

        //TODO: impl
        void Resize(int32 capacity)
        {
            PL_ASSERT(capacity >= 0);
        }

        Iterator begin() { return Iterator(Elements.begin()); }

        ConstIterator begin() const { return ConstIterator(const_cast<const TSparseArray>(Elements).begin()); }

        Iterator end() { return Iterator(Elements.end()); }

        ConstIterator end() const { return ConstIterator(const_cast<const TSparseArray>(Elements).end()); }

    private:
        template <typename InElementType>
        InElementType& Emplace(InElementType&& element)
        {
            uint32 hashCode = KeyFunc::GetHashCode(KeyFunc::GetKey(element));

            SetElementIndex index = FindIndex(KeyFunc::GetKey(element), hashCode);
            if (index.IsValid())
            {
                auto&& setElement = Elements[index.Index];
                setElement.Element.~ElementType();
                setElement.Element = element;
                return setElement.Element;
            }


            CheckRehash(Elements.GetCount() + 1);
            int32 indexInSparseArray = Elements.AddUnconstructElement();
            SetElement* item = new(Elements.GetData() + indexInSparseArray) SetElement(element);
            LinkElement(SetElementIndex{ indexInSparseArray }, *item, hashCode);
            return item->Element;
        }

        /** Contains key index in sparse array */
        template <typename KeyType>
        SetElementIndex FindIndex(const KeyType& key) const
        {
            return FindIndex(key, KeyFunc::GetHashCode(key));
        }

        /** Contains key index in sparse array */
        template <typename KeyType>
        SetElementIndex FindIndex(const KeyType& key, uint32 hashCode) const
        {
            if (Elements.GetCount() > 0)
            {
                for (SetElementIndex index = GetFirstIndex(hashCode); index.IsValid(); index = Elements[index].HashNextId)
                {
                    if (KeyFunc::Equals(KeyFunc::GetKey(Elements[index].Element), key))
                    {
                        // Return the first match, regardless of whether the set has multiple matches for the key or not.
                        return index;
                    }
                }
            }
            return SetElementIndex{};
        }

        /** Contains the head of SetElement list */
        SetElementIndex& GetFirstIndex(uint32 hashCode) const
        {
            return ((SetElementIndex*)HashBucket.GetAllocation())[GetHashIndex(hashCode)];
        }

        uint32 GetHashIndex(uint32 hashCode) const
        {
            // Quick mod hashCode % BucketCount, BucketCount must be 2 ^ n
            return hashCode & (BucketCount - 1);
        }

        bool CheckRehash(int32 elementCount)
        {
            uint32 desiredBucketCount = SetAllocator::GetNumberOfHashBuckets(static_cast<uint32>(elementCount));
            if (elementCount > 0 && (!BucketCount || BucketCount < desiredBucketCount))
            {
                BucketCount = desiredBucketCount;
                Rehash();
                return true;
            }
            return false;
        }

        void Rehash()
        {
            // Free the old hash.
            HashBucket.Resize(0, sizeof(SetElementIndex));

            if (BucketCount)
            {
                HashBucket.Resize(BucketCount, sizeof(SetElementIndex));
                // Init HashBucket
                for (uint32 hashIndex = 0; hashIndex < BucketCount; ++hashIndex)
                {
                    ((SetElementIndex*)HashBucket.GetAllocation())[hashIndex] = SetElementIndex();
                }

                // Add the existing elements to the new hash.
                for (typename TSparseArray::Iterator iter = Elements.begin(); iter != Elements.end(); ++iter)
                {
                    LinkElement(SetElementIndex(iter.GetIndex()), *iter, KeyFunc::GetHashCode(KeyFunc::GetKey((*iter).Element)));
                }
            }
        }

        void LinkElement(SetElementIndex elementIndex, SetElement& element, uint32 hashCode) const
        {
            // get the index of hash bucket.
            element.HashIndex = GetHashIndex(hashCode);

            // Link the element into the hash bucket.
            element.HashNextId = GetFirstIndex(hashCode);
            GetFirstIndex(hashCode) = elementIndex;
        }

    private:
        /** count of hash bucket */
        uint32 BucketCount{ 0 };
        DefaultSetAllocator::HashAllocator HashBucket;
        TSparseArray Elements;
    };
}

template <typename KeyType>
void* operator new(size_t size, const typename Engine::Set<KeyType>::SetElement& element)
{
    return &element.Element;
}