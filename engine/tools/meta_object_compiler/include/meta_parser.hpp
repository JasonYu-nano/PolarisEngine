#pragma once

#include "foundation/string.hpp"

namespace Engine
{
    struct ParserOptions
    {
        int32 Standard = 11;
        std::optional<String> PrecompileHeader;
    };

    class MetaParser
    {
    public:
        void Parse(const String& content, const ParserOptions& options);

        String GeneratePrecompileHeader(const String& pch);

    private:
        void ParseImpl(const String& header, const Array<const char*>& args);
    };
}
