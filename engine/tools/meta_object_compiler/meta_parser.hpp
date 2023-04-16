#pragma once

#include "foundation/string.hpp"
#include "parse_unit.hpp"
#include "misc/toml_utils.hpp"

namespace Engine
{
    struct ParserOptions
    {
        bool DisplayDebugInfo = false;
        bool IncrementalCompile = false;
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

        void ParseCursor(const CXCursor& cursor, const String& nameSpace = "");

        bool IsCursorParsed(CXCursor cursor) const;

        int64 GetLastParseTime() const;

        void UpdateLastParseTime(int64 parseTime) const;

    private:
        bool DisplayDiagnostics{ false };
        bool IncrementalCompile{ false };
        int64 ParseTime{ 0 };
        Array<RecordUnit*> Units;
        Set<String> ParsedMetaObjectNames;
        mutable toml::table Config;
    };
}
