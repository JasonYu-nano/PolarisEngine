#pragma once
#include "foundation/ustring.hpp"

namespace Engine::Private
{
    class UStringMatcher
    {
    public:
        UStringMatcher(UStringView pattern, ECaseSensitivity cs)
            : PatternView(pattern)
            , CS(cs)
        {
            UpdateSkipTable();
        }

        void UpdateSkipTable();

        strsize IndexIn(UStringView &str, strsize from = 0) const;
    private:
        ECaseSensitivity CS;
        UString Pattern;
        UStringView PatternView;
        UChar SkipTable[256] = {};
    };
}