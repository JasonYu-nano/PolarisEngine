#pragma once

#include "string_algorithm.hpp"
#include "math/city_hash.hpp"

namespace Engine
{
    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>::BasicString(CharType ch)
    {
        Append(ViewType(std::addressof(ch), 1));
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>::BasicString(const CharType* str, SizeType len)
    {
        len = len < 0 ? CharTraits::Length(str) : len;
        Append(ViewType(str, len));
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>::BasicString(SizeType count, CharType ch)
    {
        ENSURE(count > 0);
        SizeType index = AddUnconstructElement(count);
        auto data = Pair.Second().GetPtr();
        for (SizeType i = 0; i < count; ++i)
        {
            CharTraits::Assign(data[i], ch);
        }
        CharTraits::Assign(data[index + count], CharType());
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>::BasicString(const BasicString& other)
            : Pair(OneArgPlaceholder(), other.GetAllocator())
    {
        CopyAssign(other);
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>::BasicString(BasicString&& other) noexcept
            : Pair(OneArgPlaceholder(), MoveTemp(other.GetAllocator()))
    {
        MoveAssign(Forward<BasicString&&>(other));
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>::~BasicString()
    {
        if (LargeStringEngaged())
        {
            auto& myVal = Pair.Second();
            CharType* ptr = myVal.UB.Ptr;
            auto& allocator = GetAllocator();
            std::destroy_at(myVal.UB.Ptr);
            allocator.Deallocate(ptr, myVal.Size);
        }

        Invalidate();
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>& BasicString<Elem, Traits, Alloc>::operator=(const BasicString& other)
    {
        Pair.First() = other.GetAllocator();
        CopyAssign(other);
        return *this;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>& BasicString<Elem, Traits, Alloc>::operator=(BasicString&& other) noexcept
    {
        Pair.First() = MoveTemp(other.GetAllocator());
        MoveAssign(Forward<BasicString>(other));
        return *this;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>& BasicString<Elem, Traits, Alloc>::operator=(const CharType* raw)
    {
        CopyAssign(ViewType(raw));
        return *this;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::operator==(const BasicString& other) const
    {
        return Compare(other) == 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::operator!=(const BasicString& other) const
    {
        return Compare(other) != 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::operator==(const CharType* other) const
    {
        return Compare(ViewType(other)) == 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::operator!=(const CharType* other) const
    {
        return Compare(ViewType(other)) != 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::operator< (const BasicString& other) const
    {
        return Compare(static_cast<ViewType>(other)) < 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::operator< (const CharType* other) const
    {
        return Compare(ViewType(other)) < 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::operator> (const BasicString& other) const
    {
        return Compare(static_cast<ViewType>(other)) > 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::operator> (const CharType* other) const
    {
        return Compare(ViewType(other)) > 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc> BasicString<Elem, Traits, Alloc>::operator+(const BasicString& other) const
    {
        return BasicString(Data(), Length()).Append(other);
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc> BasicString<Elem, Traits, Alloc>::operator+(const CharType* other) const
    {
        return BasicString(Data(), Length()).Append(other);
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::operator+=(const BasicString& other)
    {
        Append(other);
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::operator+=(const CharType* other)
    {
        Append(other);
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc> BasicString<Elem, Traits, Alloc>::operator/ (const BasicString& other)
    {
        return BasicString(Data(), Length()).Append('/').Append(other);
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc> BasicString<Elem, Traits, Alloc>::operator/ (const CharType* other)
    {
        return BasicString(Data(), Length()).Append('/').Append(other);
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::Truncate(SizeType pos)
    {
        pos = pos < 0 ? pos + Length() : pos;
        if (pos >= 0 && pos < Length())
        {
            auto& myVal = Pair.Second();
            myVal.Size = pos + 1;
            CharTraits::Assign(myVal.GetPtr() + pos, 1, CharType());
        }
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc> BasicString<Elem, Traits, Alloc>::Slices(SizeType pos, SizeType num) const
    {
        pos = pos < 0 ? pos + Length() : pos;
        if (pos >= 0 && num >= 0 && pos + num <= Length())
        {
            return BasicString(Data() + pos, num);
        }

        return BasicString();
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::StartsWith(const ViewType& view, ECaseSensitivity cs) const
    {
        SizeType compareLen = view.Length();
        if (Empty() || Length() < compareLen)
        {
            return false;
        }

        return CharTraits::Compare(Data(), view.Data(), compareLen, cs) == 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::EndsWith(const ViewType& view, ECaseSensitivity cs) const
    {
        SizeType len = Length();
        SizeType compareLen = view.Length();
        if (Empty() || len < compareLen)
        {
            return false;
        }

        return CharTraits::Compare(Data() + len - compareLen, view.Data(), compareLen, cs) == 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>& BasicString<Elem, Traits, Alloc>::Append(const ViewType& view)
    {
        if (view.Null() || view.Empty())
        {
            return *this;
        }

        SizeType len = view.Length();
        SizeType index = AddUnconstructElement(len);
        auto data = Data();
        CharTraits::Copy(data + index, view.Data(), len);
        CharTraits::Assign(*(data + index + len), CharType());
        return *this;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>& BasicString<Elem, Traits, Alloc>::Insert(SizeType index, const ViewType& view)
    {
        if (index > Length())
        {
            return *this;
        }

        SizeType len = view.Length();
        InsertUnconstructElement(index, len);
        CharTraits::Copy(Data() + index, view.Data(), len);
        return *this;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>& BasicString<Elem, Traits, Alloc>::Remove(SizeType pos, SizeType num)
    {
        auto& myVal = Pair.Second();
        auto& size = myVal.Size;
        auto len = size - 1;
        auto data = myVal.GetPtr();

        if (pos < 0)
        {
            pos += len;
        }
        if (pos < 0 || pos >= len)
        {
        }
        else if (num >= len - pos)
        {
            Truncate(pos);
        }
        else if (num > 0)
        {
            SizeType end = pos + num - 1;
            DestructElements(data + pos, num);
            SizeType countToMove = size - end - 1;
            if (countToMove)
            {
                CharTraits::Copy(data + pos, data + end + 1, countToMove);
            }
            size -= num;
        }
        return *this;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>& BasicString<Elem, Traits, Alloc>::Remove(const ViewType& view, ECaseSensitivity cs)
    {
        SizeType from = 0;
        do
        {
            SizeType pos = FindStringHelper(static_cast<ViewType>(*this), from, view, cs);
            if (pos >= 0)
            {
                Remove(pos, view.Length());
                from = pos;
            }
            else
            {
                break;
            }
        }
        while (true);

        return *this;
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::Chop(SizeType n)
    {
        SizeType len = Length();
        if (n >= len)
        {
            Clear();
        }
        else if (n > 0)
        {
            Remove(len - n, len - 1);
        }
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc> BasicString<Elem, Traits, Alloc>::Chopped(SizeType n)
    {
        SizeType len = Length();
        if (n < 0 || n >= len)
        {
            return BasicString();
        }

        return BasicString(Data(), len - n);
    }

    template <typename Elem, typename Traits, typename Alloc>
    int32 BasicString<Elem, Traits, Alloc>::Count(const ViewType& view, ECaseSensitivity cs) const
    {
        SizeType num = 0;
        SizeType i = -1;
        while ((i = FindStringHelper(static_cast<ViewType>(*this), i + 1, view, cs)) != -1)
        {
            ++num;
        }
        return num;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>& BasicString<Elem, Traits, Alloc>::Fill(CharType ch, int32 num)
    {
        auto& myVal = Pair.Second();
        SizeType size = myVal.Size;
        if (size <= 1 && num <= 0)
        {
            return *this;
        }

        SizeType newSize = num >= 0 ? num + 1 : size;
        Reserve(newSize);
        myVal.Size = newSize;
        auto ptr = myVal.GetPtr();

        CharTraits::Assign(ptr[--newSize], CharType());
        while (newSize > 0)
        {
            --newSize;
            CharTraits::Assign(ptr[newSize], ch);
        }
        return *this;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc> BasicString<Elem, Traits, Alloc>::Repeated(int32 times) const
    {
        if (Empty() || times == 1)
        {
            return *this;
        }

        if (times <= 0)
        {
            return BasicString();
        }

        SizeType len = Length();
        SizeType resultSize = len * times;
        BasicString ret;
        ret.Reserve(resultSize + 1);
        if (ret.Capacity() > resultSize)
        {
            const CharType* raw = Data();
            while (times > 0)
            {
                ret.Append(ViewType(raw, len));
                --times;
            }
        }
        CharTraits::Assign(ret.Pair.Second().GetPtr()[resultSize], CharType());
        return ret;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc> BasicString<Elem, Traits, Alloc>::Trimmed() const
    {
        const CharType* start = Data();
        const CharType* end = Data() + Length() - 1;

        while (start < end && CharTraits::IsSpace(*start))
        {
            ++start;
        }

        while (start < end && CharTraits::IsSpace(end[-1]))
        {
            --end;
        }
        return BasicString(start, static_cast<SizeType>(end - start));
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::IsUpperLatin1() const
    {
        auto& myVal = Pair.Second();
        const CharType* data = myVal.GetPtr();
        for (SizeType i = 0; i < myVal.Size - 1; ++i)
        {
            if (CharTraits::ToUpperLatin1(data[i]) != data[i])
            {
                return false;
            }
        }
        return true;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::IsLowerLatin1() const
    {
        auto& myVal = Pair.Second();
        const CharType* data = myVal.GetPtr();
        for (SizeType i = 0; i < myVal.Size - 1; ++i)
        {
            if (CharTraits::ToLowerLatin1(data[i]) != data[i])
            {
                return false;
            }
        }
        return true;
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::ToUpperLatin1()
    {
        auto& myVal = Pair.Second();
        CharType* data = myVal.GetPtr();
        for (SizeType i = 0; i < myVal.Size - 1; ++i)
        {
            data[i] = CharTraits::ToUpperLatin1(data[i]);
        }
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::ToLowerLatin1()
    {
        auto& myVal = Pair.Second();
        CharType* data = myVal.GetPtr();
        for (SizeType i = 0; i < myVal.Size - 1; ++i)
        {
            data[i] = CharTraits::ToLowerLatin1(data[i]);
        }
    }

    template <typename Elem, typename Traits, typename Alloc>
    DynamicArray<BasicString<Elem, Traits, Alloc>>
    BasicString<Elem, Traits, Alloc>::Split(const ViewType& sep, ESplitBehavior behavior,
                                            ECaseSensitivity cs) const
    {
        DynamicArray<BasicString> ret;
        SizeType start = 0;
        SizeType end;
        while ((end = FindStringHelper(static_cast<ViewType>(*this), start, sep, cs)) != -1)
        {
            if (start != end || behavior == KeepEmptyParts)
            {
                ret.Add(Slices(start, end - start));
            }
            start = end + sep.Length();
        }
        if (start != Length() || behavior == KeepEmptyParts)
        {
            ret.Add(Slices(start, Length() - start));
        }
        return ret;
    }

    template <typename Elem, typename Traits, typename Alloc>
    DynamicArray<BasicString<Elem, Traits, Alloc>>
    BasicString<Elem, Traits, Alloc>::SplitAny(const ViewType& sep, ESplitBehavior behavior,
                                               ECaseSensitivity cs) const
    {
        DynamicArray<BasicString> ret;
        SizeType start = 0;
        SizeType end;
        while ((end = FindAnyCharHelper(static_cast<ViewType>(*this), start, sep, cs)) != -1)
        {
            if (start != end || behavior == KeepEmptyParts)
            {
                ret.Add(Slices(start, end - start));
            }
            start = end + 1;
        }
        if (start != Length() || behavior == KeepEmptyParts)
        {
            ret.Add(Slices(start, Length() - start));
        }
        return ret;
    }

    template <typename Elem, typename Traits, typename Alloc>
    uint32 BasicString<Elem, Traits, Alloc>::GetHashCode() const
    {
        return CityHash::CityHash32(Data(), Length());
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::Reserve(SizeType capacity)
    {
        auto& val = Pair.Second();
        if (capacity < val.Size || capacity <= val.MaxSize)
        {
            return;
        }

        if (!LargeStringEngaged())
        {
            BecomeLarge(capacity);
            return;
        }

        auto& alloc = GetAllocator();
        CharType* ptr = alloc.Allocate(capacity);
        CharType* oldPtr = val.UB.Ptr;
        SizeType oldSize = val.Size;
        CharTraits::Copy(ptr, oldPtr, oldSize);
        val.UB.Ptr = ptr;
        val.MaxSize = capacity;
        alloc.Deallocate(oldPtr, oldSize);
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::Invalidate()
    {
        auto& myVal = Pair.Second();
        myVal.Size = 1;
        myVal.MaxSize = INLINE_SIZE;
        myVal.UB.Buffer[0] = CharType();
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::MoveAssign(BasicString&& right)
    {
        ENSURE(std::addressof(*this) != std::addressof(right));
        auto& leftVal = Pair.Second();
        auto& rightVal = right.Pair.Second();
        int32 size = rightVal.Size;
        ENSURE(size >= 0);
        Reserve(rightVal.MaxSize);

        if constexpr (CAN_MEMORY_COPY)
        {
            const auto lhsData = reinterpret_cast<uint8*>(std::addressof(leftVal));
            const auto rhsData = reinterpret_cast<const uint8*>(std::addressof(rightVal));
            Memory::Memcpy(lhsData, rhsData, sizeof(decltype(leftVal)));
            right.Invalidate();
            return;
        }

        if (right.LargeStringEngaged())
        {
            leftVal.UB.Ptr = rightVal.UB.Ptr;
            rightVal.UB.Ptr = nullptr;
        }
        else
        {
            CharTraits::Copy(leftVal.UB.Buffer, rightVal.UB.Buffer, size);
        }

        leftVal.Size = rightVal.Size;
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::CopyAssign(const BasicString& right)
    {
        ENSURE(this->Data() != right.Data());
        auto& leftVal = Pair.Second();
        auto& rightVal = right.Pair.Second();
        SizeType size = rightVal.Size;
        ENSURE(size >= 0);
        Reserve(rightVal.MaxSize);

        CharTraits::Copy(leftVal.GetPtr(), rightVal.GetPtr(), size);
        leftVal.Size = rightVal.Size;
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::CopyAssign(const ViewType& right)
    {
        ENSURE(this->Data() != right.Data());
        auto& leftVal = Pair.Second();
        SizeType len = right.Length();
        ENSURE(len >= 0);
        Reserve(len + 1);

        CharTraits::Copy(leftVal.GetPtr(), right.Data(), len);
        CharTraits::Assign(leftVal.GetPtr()[len], CharType());
        leftVal.Size = len + 1;
    }

    template <typename Elem, typename Traits, typename Alloc>
    typename BasicString<Elem, Traits, Alloc>::SizeType BasicString<Elem, Traits, Alloc>::FindStringHelper(const ViewType& haystack, SizeType from,
                                                                                                           const ViewType& needle,
                                                                                                           ECaseSensitivity cs)
    {
        return Private::FindString<CharType, CharTraits, SizeType>(haystack.Data(), haystack.Length(), from, needle.Data(), needle.Length(), cs);
    }

    template <typename Elem, typename Traits, typename Alloc>
    typename BasicString<Elem, Traits, Alloc>::SizeType
    BasicString<Elem, Traits, Alloc>::FindAnyCharHelper(const ViewType& haystack, SizeType from,
                                                        const ViewType& needle, ECaseSensitivity cs)
    {
        const CharType* data = haystack.Data();
        for (SizeType idx = from; idx < haystack.Length(); ++idx)
        {
            if (FindStringHelper(needle, 0, ViewType(data + idx, 1), cs) > -1)
            {
                return idx;
            }
        }
        return INDEX_NONE;
    }

    template <typename Elem, typename Traits, typename Alloc>
    typename BasicString<Elem, Traits, Alloc>::SizeType
    BasicString<Elem, Traits, Alloc>::FindLastHelper(const ViewType& haystack, SizeType from,
                                                     const ViewType& needle,
                                                     ECaseSensitivity cs)
    {
        return Private::FindLastString<CharType, CharTraits, SizeType>(haystack.Data(), haystack.Length(), from, needle.Data(), needle.Length(), cs);
    }

    template <typename Elem, typename Traits, typename Alloc>
    typename BasicString<Elem, Traits, Alloc>::SizeType
    BasicString<Elem, Traits, Alloc>::FindLastAnyCharHelper(const ViewType& haystack, SizeType from,
                                                            const ViewType& needle,
                                                            ECaseSensitivity cs)
    {
        const CharType* data = haystack.Data();
        for (SizeType idx = haystack.Length() - 1; idx >= from; --idx)
        {
            if (FindStringHelper(needle, 0, ViewType(data + idx, 1), cs) > -1)
            {
                return idx;
            }
        }
        return INDEX_NONE;
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::ReplaceHelper(BasicString& source, const ViewType& before,
                                                         const ViewType& after, ECaseSensitivity cs)
    {
        if (before.Empty())
        {
            return;
        }

        SizeType alen = after.Length();
        SizeType blen = before.Length();
        if (source.Length() < blen)
        {
            return;
        }
        if (before == after)
        {
            return;
        }

        Private::StringMatcher<CharType, CharTraits, SizeType> matcher(before.Data(), blen, cs);

        DynamicArray<SizeType, InlineAllocator<128>> indices;
        SizeType pos = matcher.IndexIn(source.Data(), source.Length(), 0);

        while (pos >= 0)
        {
            indices.Add(pos);
            pos += blen;
            pos = matcher.IndexIn(source.Data(), source.Length(), pos);
        }

        int32 nIndices = indices.Size();
        if (nIndices <= 0)
        {
            return;
        }

        auto& val = source.Pair.Second();
        const CharType* afterData = after.Data();
        if (blen == alen)
        {
            CharType* src = val.GetPtr();
            for (int32 i = 0; i < nIndices; ++i)
            {
                CharTraits::Copy(src + indices[i], afterData, alen);
            }
        }
        else
        {
            SizeType oldSize = val.Size;
            int32 deltaSize = (alen - blen);
            int32 newSize = oldSize + deltaSize * nIndices;

            if (newSize > oldSize)
            {
                source.Reserve(newSize);
            }

            CharType* src = val.GetPtr();
            for (int32 i = nIndices - 1; i >= 0; --i)
            {
                SizeType idx = indices[i];
                CharTraits::Copy(src + idx + alen, src + idx + blen, oldSize - idx - blen);
                CharTraits::Copy(src + idx, afterData, alen);
            }

            CharTraits::Assign(src[newSize - 1], CharType());
            val.Size = newSize;
        }
    }

    template <typename Elem, typename Traits, typename Alloc>
    typename BasicString<Elem, Traits, Alloc>::SizeType BasicString<Elem, Traits, Alloc>::AddUnconstructElement(SizeType count)
    {
        auto& myVal = Pair.Second();
        ENSURE(count > 0 && myVal.Size > 0);
        SizeType index = myVal.Size - 1;
        SizeType newSize = myVal.Size + count;
        if (newSize > Capacity())
        {
            Expansion(newSize);
        }

        myVal.Size = newSize;
        return index;
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::InsertUnconstructElement(SizeType index, SizeType count)
    {
        auto& myVal = Pair.Second();
        ENSURE(index >= 0 && count > 0 && index <= myVal.Size);
        SizeType oldSize = myVal.Size;
        SizeType newSize = myVal.Size + count;
        if (newSize > myVal.MaxSize)
        {
            Expansion(newSize);
        }

        myVal.Size = newSize;
        CharType* src = myVal.GetPtr() + index;
        CharTraits::Copy(src + count, src, (oldSize - index));
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::DestructElements(CharType* elems, SizeType count)
    {
        bool needDestroy = LargeStringEngaged();
        while (count)
        {
            if (needDestroy)
            {
                std::destroy_at(elems);
            }
            ++elems;
            --count;
        }
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::Expansion(SizeType destSize)
    {
        SizeType newCapacity = CalculateGrowth(destSize);
        ENSURE(destSize <= newCapacity);
        Reserve(newCapacity);
    }

    template <typename Elem, typename Traits, typename Alloc>
    typename BasicString<Elem, Traits, Alloc>::SizeType BasicString<Elem, Traits, Alloc>::CalculateGrowth(const SizeType newSize) const
    {
        const auto& myVal = Pair.Second();
        SizeType oldCapacity = myVal.MaxSize;
        SizeType max = CharTraits::MaxSize();

        if (oldCapacity > max - oldCapacity / 2)
        {
            return max;
        }

        const SizeType geometric = oldCapacity + oldCapacity / 2;

        if (geometric < newSize)
        {
            return newSize;
        }

        return geometric;
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::BecomeLarge(SizeType capacity)
    {
        auto& myVal = Pair.Second();
        CharType* ptr = GetAllocator().Allocate(capacity);
        CharTraits::Copy(ptr, myVal.UB.Buffer, Size());
        myVal.UB.Ptr = ptr;
        myVal.MaxSize = capacity;
    }
}
