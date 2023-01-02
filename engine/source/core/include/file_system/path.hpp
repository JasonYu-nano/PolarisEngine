#pragma once

#include "foundation/string.hpp"

namespace Engine
{
    /**
     * @brief a static class provides helper method for path operator
     */
    class CORE_API Path
    {
    public:
        static String Combine(const String& dest, const String& part);

        static String GetExtension(const String& path);

        static String RemoveExtension(const String& path);

        /**
         * @brief get short name of file or directory
         * eg: c:/app/engine return engine
         * eg: c:/app/engine/engine.hpp return engine(.hpp)
         */
        static String GetShortName(const String& path, bool withExtension = true);

        static Array<String> SplitPath(const String& path);

        static String Normalize(const String& path);
    };
}