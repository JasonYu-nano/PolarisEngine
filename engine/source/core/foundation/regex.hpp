#pragma once
#include <regex>

/**
 * @brief wrapper of std regex, support unicode
 */

namespace Engine
{
#ifdef UNICODE
    typedef std::wregex Regex;
#else
    typedef std::regex Regex;
#endif
}
