#pragma once

#include "foundation/ustring.hpp"

namespace Engine::Private
{
    strsize FindChar(UStringView target, strsize from, UChar ch, ECaseSensitivity cs);

    strsize FindString(UStringView target, strsize from, UStringView sub, ECaseSensitivity cs);

    strsize FindString(UStringView target, strsize from, StringView sub, ECaseSensitivity cs);

    strsize FindStringBoyerMoore(UStringView target, strsize from, UStringView sub, ECaseSensitivity cs);
}