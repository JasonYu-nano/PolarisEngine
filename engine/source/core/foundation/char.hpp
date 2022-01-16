#pragma once

#include "predefine/platform.hpp"

namespace Engine
{
    template <typename CharType>
    class Char
    {
    public:
        static CharType ToUpper(CharType character)
        {
            return (CharType)(ToUnsigned(character) - ((uint32(character) - 'a' < 26u) << 5));
        }

        static CharType ToLower(CharType character)
        {
            return (CharType)(ToUnsigned(character) + ((uint32(character) - 'A' < 26u) << 5));
        }

        static constexpr uint32 ToUnsigned(CharType character)
        {
            return (uint32)character;
        }
    };
}
