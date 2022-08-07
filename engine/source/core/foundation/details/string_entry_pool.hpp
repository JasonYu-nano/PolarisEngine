#pragma once

#include <shared_mutex>
#include "foundation/map.hpp"
#include "foundation/string.hpp"
#include "foundation/ustring.hpp"
#include "foundation/char_utils.hpp"
#include "math/limit.hpp"
#include "math/city_hash.hpp"

namespace Engine
{
    #define SUFFIX_NUMBER_NONE (0)
    #define ACTUAL_TO_SUFFIX(number) ((number) + 1)
    #define SUFFIX_TO_ACTUAL(number) ((number) - 1)

    #define MAX_ENTRY_LENGTH (1024)

    constexpr int32 InitialBucketCount = 1 << 9;

    using FixedStringView = BasicStringView<char_t>;

    class FixedStringHelper
    {
    public:
        static constexpr int64 Atoi64(const UChar* str, int32 length)
        {
            int64 number = 0;
            for (int32 Idx = 0; Idx < length; ++Idx)
            {
                number = 10 * number + static_cast<int32>(str[Idx]) - '0';
            }

            return number;
        }

        static uint32 SplitNumber(UStringView& view)
        {
            const UChar* str = view.Data();
            strsize& len = view.Length();

            int32 numberCount = 0;
            for (const UChar* pChar = str + len - 1; pChar >= str && *pChar >= '0' && *pChar <= '9'; --pChar )
            {
                ++numberCount;
            }

            // number suffix must start with _ and skip number like 01
            const UChar* firstNumber = str + len - numberCount;
            if (numberCount > 0 && numberCount < len && *(firstNumber - 1) == '_')
            {
                if (numberCount == 1 || *firstNumber != '0')
                {
                    int64 number = Atoi64(firstNumber, numberCount);
                    if (number < MAX_UINT32)
                    {
                        len -= (numberCount + 1);
                        return static_cast<uint32>(ACTUAL_TO_SUFFIX(number));
                    }
                }
            }

            return SUFFIX_NUMBER_NONE;
        }
    };

    using StringEntryPoolAllocator = SetAllocator<DefaultAllocator, DefaultAllocator, 2, InitialBucketCount, 4>;

#ifdef SMALLER_FIXED_STRING
    using FixedEntryId = uint32;
#else
    using FixedEntryId = uint64;
#endif

    class StringEntryPool
    {
        using TEntryPool = Map<FixedEntryId, FixedStringView, MapDefaultHashFunc<FixedEntryId, FixedStringView>, StringEntryPoolAllocator>;
        using EntryPoolType = Map<FixedEntryId, UString, MapDefaultHashFunc<FixedEntryId, UString>, StringEntryPoolAllocator>;
    public:
        static StringEntryPool& Get()
        {
            static StringEntryPool inst;
            return inst;
        }

        static FixedEntryId AllocEntryId(const FixedStringView& entry);

        static FixedEntryId AllocEntryId(const UStringView& entry);

        FixedEntryId FindOrStore(const UStringView& entry);

        FixedEntryId Store(const UStringView& entry);

        UString* Find(FixedEntryId entryId);

    private:
        template <typename CharType>
        static FixedEntryId GetLowerCaseHash(const CharType* str, int32 length)
        {
            CharType lowerStr[MAX_ENTRY_LENGTH];
            for (int32 idx = 0; idx < length; ++idx)
            {
                lowerStr[idx] = CharTraits<CharType>::Latin1ToLower(str[idx]);
            }
            #ifdef SMALLER_FIXED_STRING
            return CityHash::CityHash32(reinterpret_cast<const char*>(lowerStr), length * sizeof(CharType));
            #else
            return CityHash::CityHash64(reinterpret_cast<const char*>(lowerStr), length * sizeof(CharType));
            #endif
        }

        static FixedEntryId GetLowerCaseHash(UStringView view)
        {
            UChar lowerStr[MAX_ENTRY_LENGTH];
            strsize len = view.Length();
            const UChar* data = view.Data();
            for (strsize idx = 0; idx < len; ++idx)
            {
                lowerStr[idx] = CharTraits<UChar>::Latin1ToLower(static_cast<char16_t>(data[idx]));
            }
#ifdef SMALLER_FIXED_STRING
            return CityHash::CityHash32(reinterpret_cast<const char*>(lowerStr), len * sizeof(UChar));
#else
            return CityHash::CityHash64(reinterpret_cast<const char*>(lowerStr), len * sizeof(UChar));
#endif
        }
    private:
        std::shared_mutex Mutex;
        EntryPoolType EntryPool;
    };
}