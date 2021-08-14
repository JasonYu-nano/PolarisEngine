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
        static String MakePath(const String& dest, const String& part);

        static String GetExtension(const String& path);

        static String RemoveExtension(const String& path);

        /**
         * @brief get short name of file or directory
         * eg: c:/app/nano_engine return nano_engine
         * eg: c:/app/nano_engine/engine.hpp return engine(.hpp)
         */
        static String GetShortName(const String& path, bool withExtension = true);

        static Vector<String> SplitPath(const String& path);
    };
}