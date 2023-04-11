#pragma once

#include "code_writer.hpp"

namespace Engine
{
    class ICodeGenerator
    {
    public:
        virtual void GenerateCode(CodeWriter& headerWriter, CodeWriter& sourceWriter) const = 0;
    };
}
