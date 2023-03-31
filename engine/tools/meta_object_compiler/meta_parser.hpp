#pragma once

#include "foundation/string.hpp"
#include "parse_unit.hpp"

namespace Engine
{
    struct ParserOptions
    {
        int32 Standard = 11;
        String PrecompileHeader;
        Set<String> IncludeDirs;
    };

    class MetaParser
    {
    public:
        using Iterator = Array<RecordUnit*>::Iterator;
        using ConstIterator = Array<RecordUnit*>::ConstIterator;

        ~MetaParser();

        void Parse(const Set<String>& content, const ParserOptions& options);

        String GeneratePrecompileHeader(const String& pch);

        Iterator begin() { return Units.begin(); }

        ConstIterator begin() const  { return Units.begin(); }

        Iterator end()  { return Units.end(); }

        ConstIterator end() const  { return Units.end(); }

    private:
        void ParseImpl(const String& header, const Array<const char*>& args);

        void ParseCursor(const CXCursor& cursor, const String& header, const String& nameSpace = "");

        bool IsCursorParsed(CXCursor cursor) const;

        Array<RecordUnit*> Units;

        Set<String> ParsedMetaObjectNames;
    };
}
