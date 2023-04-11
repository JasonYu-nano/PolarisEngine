#pragma once

#include "property_unit.hpp"

namespace Engine
{
    class StructUnit : public RecordUnit
    {
        using Super = RecordUnit;
    public:
        StructUnit(CXCursor cursor, const String& path, const String& nameSpace) : Super(cursor, path, nameSpace)
        {
            RecordType = ERecordType::Struct;
        }

        void Parse() final;

        void GenerateCode(CodeWriter& headerWriter, CodeWriter& sourceWriter) const override;

    private:
        Set<String> GetSupportFlags() override
        {
            return Set<String>();
        }

    private:
        String SuperName;
        Array<PropertyUnit> Properties;
    };
}
