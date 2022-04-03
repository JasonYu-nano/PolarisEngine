#include "windows/windows_platform_string.hpp"
#include <string>

namespace Engine
{

    wchar* WindowsPlatformString::A2W(const ansi* original)
    {
        const size_t newSize = strlen(original) + 1;
        wchar* ret = new wchar[newSize];

        size_t convertedChars = 0;
        mbstowcs_s(&convertedChars, ret, newSize, original, _TRUNCATE);
        return ret;
    }

    ansi* WindowsPlatformString::W2A(const wchar* original)
    {
        const size_t origSize = wcslen(original) + 1;
        const size_t newSize = origSize * 2;
        ansi* ret = new ansi[newSize];

        size_t convertedChars = 0;
        wcstombs_s(&convertedChars, ret, newSize, original, _TRUNCATE);
        return ret;
    }
}