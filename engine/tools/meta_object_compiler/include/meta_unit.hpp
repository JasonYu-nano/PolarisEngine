#pragma once

#include "clang-c/Index.h"
#include "foundation/string.hpp"
#include "foundation/map.hpp"

namespace Engine
{
    class MetaUnit
    {
    public:
        MetaUnit(const CXCursor& cursor, const String& path)
            : MyCursor(cursor)
            , Path(path)
        {}

        virtual void Parse();

        virtual void GenerateMetaData() = 0;

    protected:
        void ParseMetaAttribute(const CXCursor& cursor, Set<String>& flags, Map<String, String>& attributes);

    private:
        virtual Set<String> GetSupportFlags() = 0;

        virtual Set<String> GetSupportAttributeNames() = 0;

    protected:
        CXCursor MyCursor;
        String Path;
        Set<String> Flags;
        Map<String, String> Attributes;
    };
}
