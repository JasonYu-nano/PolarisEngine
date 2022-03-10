#pragma once

#include "foundation/map.hpp"
#include "foundation/fixed_string.hpp"
#include "reflection/method.hpp"
#include "reflection/field.hpp"

namespace Engine
{
    class Structure
    {
    public:
        Method& AddMethod(Method&& method)
        {
            return Methods.Add(method.GetName(), method);
        }

        Field& AddField(Field&& field)
        {
            return Fields.Add(field.GetName(), field);
        }

    private:
        Map<FixedString, Method> Methods;

        Map<FixedString, Field> Fields;
    };
}