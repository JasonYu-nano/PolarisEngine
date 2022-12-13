#pragma once

#include <shared_mutex>
#include "foundation/map.hpp"
#include "foundation/string.hpp"
#include "math/limit.hpp"
#include "math/city_hash.hpp"

namespace Engine
{
    #define SUFFIX_NUMBER_NONE (0)
    #define ACTUAL_TO_SUFFIX(number) ((number) + 1)
    #define SUFFIX_TO_ACTUAL(number) ((number) - 1)

    #define MAX_ENTRY_LENGTH (1024)

    constexpr int32 InitialBucketCount = 1 << 9;

    class FixedStringHelper
    {
    public:
        static constexpr int64 Atoi64(const char* str, int32 length)
        {
            int64 number = 0;
            for (int32 Idx = 0; Idx < length; ++Idx)
            {
                number = 10 * number + static_cast<int32>(str[Idx]) - '0';
            }

            return number;
        }

        static uint32 SplitNumber(StringView& view)
        {
            const char* str = view.Data();
            int32& len = view.Length();

            int32 numberCount = 0;
            for (const char* pChar = str + len - 1; pChar >= str && int32(*pChar) >= '0' && int32(*pChar) <= '9'; --pChar )
            {
                ++numberCount;
            }

            // number suffix must start with _ and skip number like 01
            const char* firstNumber = str + len - numberCount;
            if (numberCount > 0 && numberCount < len && int32(*(firstNumber - 1)) == '_')
            {
                if (numberCount == 1 || int32(*firstNumber) != '0')
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
        using EntryPoolType = Map<FixedEntryId, String, MapDefaultHashFunc<FixedEntryId, String>, StringEntryPoolAllocator>;
    public:
        static StringEntryPool& Get()
        {
            static StringEntryPool inst;
            return inst;
        }

        static FixedEntryId AllocEntryId(const StringView& entry);

        FixedEntryId FindOrStore(const StringView& entry);

        FixedEntryId Store(const StringView& entry);

        String* Find(FixedEntryId entryId);

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

        static FixedEntryId GetLowerCaseHash(StringView view)
        {
            char lowerStr[MAX_ENTRY_LENGTH];
            int32 len = view.Length();
            const char* data = view.Data();
            for (int32 idx = 0; idx < len; ++idx)
            {
                lowerStr[idx] = CharTraits<char>::Latin1ToLower(data[idx]);
            }
#ifdef SMALLER_FIXED_STRING
            return CityHash::CityHash32(reinterpret_cast<const char*>(lowerStr), len * sizeof(char));
#else
            return CityHash::CityHash64(reinterpret_cast<const char*>(lowerStr), len * sizeof(char));
#endif
        }
    private:
        std::shared_mutex Mutex;
        EntryPoolType EntryPool;
    };
}