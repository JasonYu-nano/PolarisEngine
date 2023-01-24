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

#ifdef WITH_EDITOR
#define STRING_ID_CASE_SENSITIVE 1
#else
#define STRING_ID_CASE_SENSITIVE 0
#endif

    class StringIDHelper
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

    struct StringEntryID
    {
#if STRING_ID_CASE_SENSITIVE
        uint32 DisplayID = 0;
#endif
        uint32 CompressID = 0;
    };

    class StringEntryPool
    {
    public:
        static StringEntryPool& Get()
        {
            static StringEntryPool inst;
            return inst;
        }

        static StringEntryID AllocEntryID(const StringView& entry);

        StringEntryID FindOrStore(const StringView& entry);

        void Store(StringEntryID id, const StringView& entry);

        String* Find(StringEntryID id);

        static uint32 CalcCompressID(const StringView& view);

        static uint32 CalcDisplayID(const StringView& view);

    private:
        std::shared_mutex Mutex;
        Map<uint32, String> EntryPool;
    };
}