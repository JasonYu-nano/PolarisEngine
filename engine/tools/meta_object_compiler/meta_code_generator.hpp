#pragma once

#include "meta_parser.hpp"
#include "code_generator_interface.hpp"

namespace Engine
{
    class MetaCodeGenerator
    {
    public:
        MetaCodeGenerator(const MetaParser& parser) : Parser(parser) {}

        void GenerateCode();

    private:
        const MetaParser& Parser;
    };
}
