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
        bool IsValid() const { return Index == kSparseArrayIndexNone; }

        operator uint32() const
        {
            return Index;
        }

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
            SetElementIndex NextIndex = 0;
            KeyType Element;
        };

        using TSparseArray = SparseArray<SetElement>;

    public:
        void Add(const KeyType& key)
        {
            uint32 hashCode = HashFunc.GetHashCode(key);
            Emplace(hashCode, key);
        }

        bool Find(const KeyType& key) const
        {
            return FindIndex(key).IsValid();;
        }
    private:
        template <typename... Args>
        void Emplace(uint32 hashCode, Args&&... args)
        {
            SetElementIndex index = FindIndex(hashCode);
            if (index.IsValid())
            {
                return;
            }

            if (CheckRehash(Elements.GetCount() + 1))
            {
                uint32 index = Elements.Add(SetElement());
                Elements[index].HashIndex = GetHashIndex(hashCode);
                Elements[index].HashIndex = GetHashIndex(hashCode);
                LinkElement()
            }
            uint32 index = Elements.AddUnconstructElement();
            SetElement* element = new(Elements.GetData() + index) KeyType(Forward<Args>(args)...);
            LinkElement(SetElementIndex{ Index = index }, element, hashCode);
        }

        /** find key index in sparse array */
        SetElementIndex FindIndex(const KeyType& key) const
        {
            return FindIndex(HashFunc.GetHashCode(key));
        }

        /** find key index in sparse array */
        SetElementIndex FindIndex(uint32 hashCode) const
        {
            if (Elements.GetCount())
            {
                for (SetElementIndex index = GetFirstIndex(hashCode); index.IsValid(); index = Elements[index].NextIndex)
                {
                    if (Elements[index].Element == Key)
                    {
                        // Return the first match, regardless of whether the set has multiple matches for the key or not.
                        return index;
                    }
                }
            }
            return SetElementIndex();
        }

        SetElementIndex& GetFirstIndex(int32 hashCode) const
        {
            // quick mod hashCode % BucketCount, BucketCount must be 2 ^ n
            return ((SetElementIndex*)HashBucket.GetAllocation())[GetHashIndex(hashCode)];
        }

        uint32 GetHashIndex(int32 hashCode) const
        {
            return hashCode & (BucketCount - 1);
        }

        bool CheckRehash(uint32 elementCount)
        {
            uint32 desiredBucketCount = SetAllocator.GetNumberOfHashBuckets(elementCount);
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
            HashBucket.Resize(0, 0, sizeof(SetElementIndex));

            int32 LocalHashSize = BucketCount;
            if (LocalHashSize)
            {
                HashBucket.Resize(0, BucketCount, sizeof(SetElementIndex));
                for (int32 HashIndex = 0; HashIndex < BucketCount; ++HashIndex)
                {
                    GetFirstIndex(HashIndex) = SetElementIndex();
                }

                // Add the existing elements to the new hash.
                for (typename TSparseArray::ConstIterator iter = Elements.begin(); iter != Elements.end(); ++iter)
                {
                    LinkElement(SetElementIndex(iter.GetIndex()), *iter, HashFunc.GetHashCode(iter->Element));
                }
            }
        }

        void LinkElement(SetElementIndex elementIndex, const SetElement& element, uint32 hashCode) const
        {
            // get the index of hash bucket.
            element.HashIndex = GetHashIndex(hashCode);

            // Link the element into the hash bucket.
            element.HashNextId = GetFirstIndex(hashCode);
            GetFirstIndex(hashCode) = elementIndex;
        }

        friend void* operator new(size_t size, const SetElement& element)
        {
            return &element.Element;
        }

    private:
        /** count of hash bucket */
        uint32 BucketCount{ 0 };
        DefaultSetAllocator::HashAllocator HashBucket;
        TSparseArray Elements;
    };
}