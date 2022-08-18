#pragma once

#include "foundation/ustring.hpp"

namespace Engine
{
    /**
     * @brief a static class provides helper method for path operator
     */
    class CORE_API Path
    {
    public:
        static UString Combine(const UString& dest, const UString& part);

        static UString GetExtension(const UString& path);

        static UString RemoveExtension(const UString& path);

        /**
         * @brief get short name of file or directory
         * eg: c:/app/engine return engine
         * eg: c:/app/engine/engine.hpp return engine(.hpp)
         */
        static UString GetShortName(const UString& path, bool withExtension = true);

        static DynamicArray<UString> SplitPath(const UString& path);

        static UString Normalize(const UString& path);
    };
}