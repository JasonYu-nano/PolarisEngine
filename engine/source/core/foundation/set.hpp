#pragma once

#include "misc/type_hash.hpp"
#include "foundation/sparse_array.hpp"
#include "foundation/stl_misc.hpp"

namespace Engine
{
    template <typename Type>
    struct DefaultHashFunc
    {
        static uint32 GetHashCode(const Type& value)
        {
            return ::Engine::GetHashCode(value);
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
        bool IsValid() const { return Index != kSparseArrayIndexNone; }

        operator uint32() const
        {
            return Index;
        }

        // Index in sparse array
        uint32 Index{ kSparseArrayIndexNone };
    };

    using DefaultSetAllocator = SetAllocator<HeapAllocator<uint32>, HeapAllocator<uint32>>;

    template <typename KeyType, typename HashFunc = DefaultHashFunc<KeyType>, typename SetAllocator = DefaultSetAllocator>
    class Set
    {
        struct SetElement
        {
            SetElement(const KeyType& element)
            {
                Element = element;
            }

            SetElement(KeyType&& element)
            {
                Element = element;
            }

            uint32 HashIndex = 0;
            SetElementIndex HashNextId = 0;
            KeyType Element;
        };

        using TSparseArray = SparseArray<SetElement>;

    public:
        Set() = default;

        /**
         * Adds the specified element to Set.
         * 
         * @param T key
         */
        void Add(const KeyType& key)
        {
            Emplace(key);
        }

        /**
         * Moves the specified element to Set.
         *
         * @param T key
         */
        void Add(KeyType&& key)
        {
            Emplace(key);
        }

        /**
         * Determines whether a Set contains the specified element.
         * 
         * @param T key
         * @return boolean true if contains the specified element, otherwise false.
         */
        bool Contains(const KeyType& key) const
        {
            return FindIndex(key).IsValid();
        }

        /**
         * Removes the specified element from a Set.
         * 
         * @param T key
         * @return boolean true if remove success.
         */
        bool Remove(const KeyType& key)
        {
            SetElementIndex* index = &GetFirstIndex(HashFunc::GetHashCode(key));
            bool ret = false;
            while (index->IsValid())
            {
                auto&& setElement = Elements[index->Index];
                if (setElement.Element == key)
                {
                    uint32 pendingRemoveIndex = index->Index;
                    index->Index = setElement.HashNextId.Index;
                    Elements.RemoveAt(pendingRemoveIndex);
                    ret = true;
                    break;
                }
                else
                {
                    index = &setElement.HashNextId;
                }
            }
            return ret;
        }
    private:
        template <typename ElementType>
        void Emplace(ElementType&& key)
        {
            uint32 hashCode = HashFunc::GetHashCode(key);
            SetElementIndex index = FindIndex(key, hashCode);
            if (index.IsValid())
            {
                return;
            }

            CheckRehash(Elements.GetCount() + 1);
            uint32 indexInSparseArray = Elements.AddUnconstructElement();
            SetElement* element = new(Elements.GetData() + indexInSparseArray) SetElement(key);
            LinkElement(SetElementIndex{ indexInSparseArray }, *element, hashCode);
        }

        /** Contains key index in sparse array */
        SetElementIndex FindIndex(const KeyType& key) const
        {
            return FindIndex(key, HashFunc::GetHashCode(key));
        }

        /** Contains key index in sparse array */
        SetElementIndex FindIndex(const KeyType& key, uint32 hashCode) const
        {
            if (Elements.GetCount())
            {
                for (SetElementIndex index = GetFirstIndex(hashCode); index.IsValid(); index = Elements[index].HashNextId)
                {
                    if (Elements[index].Element == key)
                    {
                        // Return the first match, regardless of whether the set has multiple matches for the key or not.
                        return index;
                    }
                }
            }
            return SetElementIndex();
        }

        /** Contains the head of SetElement list */
        SetElementIndex& GetFirstIndex(uint32 hashCode) const
        {
            return ((SetElementIndex*)HashBucket.GetAllocation())[GetHashIndex(hashCode)];
        }

        uint32 GetHashIndex(int32 hashCode) const
        {
            // Quick mod hashCode % BucketCount, BucketCount must be 2 ^ n
            return hashCode & (BucketCount - 1);
        }

        bool CheckRehash(uint32 elementCount)
        {
            uint32 desiredBucketCount = SetAllocator::GetNumberOfHashBuckets(elementCount);
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
                    LinkElement(SetElementIndex(iter.GetIndex()), *iter, HashFunc::GetHashCode((*iter).Element));
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