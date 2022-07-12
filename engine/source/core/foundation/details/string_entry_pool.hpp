#pragma once

#include <shared_mutex>
#include "foundation/map.hpp"
#include "foundation/string.hpp"
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

    using FixedStringView = StringView<char_t, int32>;

    class FixedStringHelper
    {
    public:
        template <typename CharType>
        static constexpr int64 Atoi64(const CharType* str, int32 length)
        {
            int64 number = 0;
            for (int32 Idx = 0; Idx < length; ++Idx)
            {
                number = 10 * number + str[Idx] - '0';
            }

            return number;
        }

        template <typename CharType>
        static uint32 SplitNumber(CharType* str, int32& length)
        {
            int32 numberCount = 0;
            for (CharType* pChar = str + length - 1; pChar >= str && *pChar >= '0' && *pChar <= '9'; --pChar )
            {
                ++numberCount;
            }

            // number suffix must start with _ and skip number like 01
            const CharType* firstNumber = str + length - numberCount;
            if (numberCount > 0 && numberCount < length && *(firstNumber - 1) == '_')
            {
                if (numberCount == 1 || *firstNumber != '0')
                {
                    int64 number = Atoi64(firstNumber, numberCount);
                    if (number < MAX_UINT32)
                    {
                        length -= (numberCount + 1);
                        return static_cast<uint32>(ACTUAL_TO_SUFFIX(number));
                    }
                }
            }

            return SUFFIX_NUMBER_NONE;
        }
    };

    using StringEntryPoolAllocator = SetAllocator<HeapAllocator<uint32>, HeapAllocator<uint32>, 2, InitialBucketCount, 4>;

#ifdef SMALLER_FIXED_STRING
    using FixedEntryId = uint32;
#else
    using FixedEntryId = uint64;
#endif

    class StringEntryPool
    {
        using TEntryPool = Map<uint64, FixedStringView, MapDefaultHashFunc<uint64, FixedStringView>, StringEntryPoolAllocator>;
    public:
        static StringEntryPool& Get()
        {
            static StringEntryPool inst;
            return inst;
        }
        static FixedEntryId AllocEntryId(const FixedStringView& entry);

        FixedEntryId FindOrStore(const FixedStringView& entry);

        FixedEntryId Store(const FixedStringView& entry);

        FixedStringView* Find(FixedEntryId entryId);

    private:
        template <typename CharType>
        static FixedEntryId GetLowerCaseHash(const CharType* str, int32 length)
        {
            CharType lowerStr[MAX_ENTRY_LENGTH];
            for (int32 idx = 0; idx < length; ++idx)
            {
                lowerStr[idx] = CharUtils::ToLower(str[idx]);
            }
            #ifdef SMALLER_FIXED_STRING
            return CityHash::CityHash32(reinterpret_cast<const char*>(lowerStr), length * sizeof(CharType));
            #else
            return CityHash::CityHash64(reinterpret_cast<const char*>(lowerStr), length * sizeof(CharType));
            #endif
        }
    private:
        std::shared_mutex Mutex;
        TEntryPool EntryPool;
    };
}