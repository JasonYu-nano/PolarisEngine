#pragma once

#include "meta_parser.hpp"
#include "code_generator_interface.hpp"

namespace Engine
{
    class MetaCodeGenerator
    {
    public:
        MetaCodeGenerator(const MetaParser& parser) : Parser(parser) {}

        void Generate();

    private:
        void GenerateCode();

        void GenerateManifest();

        void CleanupContent();
    private:
        const MetaParser& Parser;
        Map<StringID, Set<String>> TargetDependenceFiles;
    };
}
