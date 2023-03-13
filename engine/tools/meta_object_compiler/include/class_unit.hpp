#pragma once

#include "property_unit.hpp"
#include "method_unit.hpp"

namespace Engine
{
    class ClassUnit : public MetaUnit
    {
        using Super = MetaUnit;
    public:
        ClassUnit(const CXCursor& cursor, const String& path) : Super(cursor, path) {}

        void Parse() final;

        void GenerateMetaData() override
        {
        }

    private:
        Set<String> GetSupportFlags() override;

        Set<String> GetSupportAttributeNames() override;

    private:
        Array<PropertyUnit> Properties;
        Array<MethodUnit> Methods;
    };
}
