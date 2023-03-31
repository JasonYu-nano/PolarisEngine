#include "meta_enum.hpp"

int64 MetaEnum::GetEnumConstantValueByIndex(int32 index) const
{
    if (ConstantDecl.IsValidIndex(index))
    {
        return ConstantDecl[index].ConstantValue;
    }

    return INDEX_NONE;
}

StringID MetaEnum::GetEnumConstantNameByIndex(int32 index) const
{
    if (ConstantDecl.IsValidIndex(index))
    {
        return ConstantDecl[index].ConstantName;
    }

    return "";
}

#if WITH_META_DATA
String MetaEnum::GetEnumConstantDisplayNameByIndex(int32 index) const
{
    if (!ConstantDecl.IsValidIndex(index))
    {
        return "";
    }

    const MetaEnumConstantDecl& decl = ConstantDecl[index];

    String* displayName = decl.MetaDataMap.Find("DisplayName");
    if (displayName)
    {
        return *displayName;
    }

    return decl.ConstantName.ToString();
}
#endif

MetaEnumConstantDecl& MetaEnum::AddConstantDecl(MetaEnumConstantDecl&& decl)
{
    ConstantDecl.Add(decl);
    return ConstantDecl.Last();
}
