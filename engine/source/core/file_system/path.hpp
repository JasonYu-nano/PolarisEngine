#pragma once
#include "foundation/string.hpp"
#include "foundation/ustring.hpp"

namespace Engine
{
    /**
     * @brief a static class provides helper method for path operator
     */
    class CORE_API Path
    {
    public:
        static String MakePath(const String& dest, const String& part);

        static UString MakePath(const UString& dest, const UString& part);

        static String GetExtension(const String& path);

        static UString GetExtension(const UString& path);

        static String RemoveExtension(const String& path);

        static UString RemoveExtension(const UString& path);

        /**
         * @brief get short name of file or directory
         * eg: c:/app/nano_engine return nano_engine
         * eg: c:/app/nano_engine/engine.hpp return engine(.hpp)
         */
        static String GetShortName(const String& path, bool withExtension = true);
        static UString GetShortName(const UString& path, bool withExtension = true);

        static DynamicArray<String> SplitPath(const String& path);

        static DynamicArray<UString> SplitPath(const UString& path);
    };
}