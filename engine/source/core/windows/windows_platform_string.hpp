#pragma once

#include "predefine/platform.hpp"

namespace Engine
{
    class WindowsPlatformString
    {
    public:
        static wchar* A2W(const ansi* original);

        static ansi* W2A(const wchar* original);
    };

    typedef WindowsPlatformString PlatformString;
}