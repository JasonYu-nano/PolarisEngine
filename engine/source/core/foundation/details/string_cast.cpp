#include "foundation/string_cast.hpp"
#include "predefine/prerequisite.hpp"

#if PLATFORM_WINDOWS
#include "windows/windows_platform_string.hpp"
#else

#endif

namespace Engine
{
    const wchar* A2W(const ansi* original)
    {
        return PlatformString::A2W(original);
    }

    const ansi* W2A(const wchar* original)
    {
        return PlatformString::W2A(original);
    }
}