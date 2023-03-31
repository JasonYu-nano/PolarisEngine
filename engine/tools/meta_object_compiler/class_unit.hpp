#pragma once

#include "property_unit.hpp"
#include "method_unit.hpp"

namespace Engine
{
    class ClassUnit : public RecordUnit
    {
        using Super = RecordUnit;
    public:
        ClassUnit(CXCursor cursor, const String& path, const String& nameSpace) : Super(cursor, path, nameSpace)
        {
            RecordType = ERecordType::Class;
        }

        void Parse() final;

        void GenerateCode(CodeWriter& writer) const override;

        bool HasMethod(const String& methodName) const;

    private:
        Set<String> GetSupportFlags() override;

    private:
        String SuperName;
        Array<PropertyUnit> Properties;
        Array<MethodUnit> Methods;
    };
}
