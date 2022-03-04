#pragma once

#include "foundation/map.hpp"
#include "foundation/fixed_string.hpp"
#include "reflection/method.hpp"
#include "reflection/field.hpp"

namespace Engine
{
    class Structure
    {
    private:
        Map<FixedString, Method> Methods;

        Map<FixedString, Field> Fields;
    };
}