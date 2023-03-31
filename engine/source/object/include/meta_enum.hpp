#pragma once

#include "global.hpp"
#include "log/logger.hpp"
#include "foundation/string_id.hpp"
#include "foundation/map.hpp"
#include "meta_attributes.hpp"

namespace Engine
{
    class OBJECT_API MetaEnumConstantDecl : public MetaObject
    {
        friend class MetaEnum;
    public:
        MetaEnumConstantDecl(StringID constantName, int64 constantValue, EMetaFlag flags)
            : ConstantName(constantName)
            , ConstantValue(constantValue)
        {
            Flags = flags;
        }

    private:
        StringID ConstantName;
        int64 ConstantValue;
    };

    class OBJECT_API MetaEnum : public MetaObject
    {
        friend struct MetaConstructUtils;
    public:
        MetaEnum(StringID enumName, EMetaFlag flags)
            : EnumName(enumName)
        {
            Flags = flags;
        }

        int32 GetEnumSize() const { return ConstantDecl.Size(); }

        int64 GetEnumConstantValueByIndex(int32 index) const;

        StringID GetEnumConstantNameByIndex(int32 index) const;

#if WITH_META_DATA
        String GetEnumConstantDisplayNameByIndex(int32 index) const;
#endif

    private:
        MetaEnumConstantDecl& AddConstantDecl(MetaEnumConstantDecl&& decl);

    private:
        StringID EnumName;
        Array<MetaEnumConstantDecl> ConstantDecl;
    };

    template <typename T>
    MetaEnum* GetEnum()
    {
        static_assert(FalseExpr<T>, "Can't match MetaEnum");
        return nullptr;
    }
}
