#pragma once
#include <string_view>
#include "predefine/platform.hpp"

namespace Engine
{
    template <typename CharType, typename SizeType>
    struct StringView
    {
        StringView() = default;

        StringView(const CharType* str, SizeType length)
            : Length(length)
            , Data(str)
        {}

        SizeType Length = 0;
        const CharType* Data = nullptr;
    };

    using TStringView = StringView<char_t, size_t>;
}