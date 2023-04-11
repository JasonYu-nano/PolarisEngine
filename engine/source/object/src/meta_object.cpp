#include "meta_object.hpp"

bool MetaClass::IsChildOf(MetaClass* metaClass) const
{
    if (this == metaClass)
    {
        return false;
    }

    MetaClass* super = SuperClass;
    while (super)
    {
        if (super == metaClass)
        {
            return true;
        }
        super = super->SuperClass;
    }

    return false;
}

bool MetaClass::FindProperty(StringID name, MetaProperty*& property)
{
    int32 index = Properties.Find([&name](const MetaProperty& elem){
        return elem.GetPropertyName() == name;
    });

    if (index == INDEX_NONE)
    {
        property = nullptr;
        return false;
    }

    property = &Properties[index];
    return true;
}

bool MetaClass::FindMethod(StringID name, MetaMethod* method)
{
    method = Methods.Find(name);
    return method != nullptr;
}

MetaProperty* MetaClass::AddProperty(MetaProperty&& property)
{
    Properties.Add(std::forward<MetaProperty>(property));
    return &Properties.Last();
}

MetaMethod* MetaClass::AddMethod(MetaMethod&& method)
{
    return &Methods.Add(method.GetMethodName(), std::forward<MetaMethod>(method));
}

bool MetaStruct::IsChildOf(MetaStruct* metaStruct) const
{
    if (this == metaStruct)
    {
        return false;
    }

    MetaStruct* super = SuperStruct;
    while (super)
    {
        if (super == metaStruct)
        {
            return true;
        }
        super = super->SuperStruct;
    }

    return false;
}

bool MetaStruct::FindProperty(StringID name, MetaProperty*& property)
{
    int32 index = Properties.Find([&name](const MetaProperty& elem){
        return elem.GetPropertyName() == name;
    });

    if (index == INDEX_NONE)
    {
        property = nullptr;
        return false;
    }

    property = &Properties[index];
    return true;
}

MetaProperty* MetaStruct::AddProperty(MetaProperty&& property)
{
    Properties.Add(std::forward<MetaProperty>(property));
    return &Properties.Last();
}

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