#include "meta_object.hpp"

GMetaClass::~GMetaClass()
{
    for (auto property : Properties)
    {
        delete property;
    }
    Properties.Clear();
}

bool GMetaClass::IsChildOf(GMetaClass* metaClass) const
{
    if (this == metaClass)
    {
        return false;
    }

    GMetaClass* super = SuperClass;
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

bool GMetaClass::FindProperty(StringID name, MetaProperty*& property)
{
    int32 index = Properties.Find([&name](MetaProperty* elem){
        return elem->GetPropertyName() == name;
    });

    if (index == INDEX_NONE)
    {
        property = nullptr;
        return false;
    }

    property = Properties[index];
    return true;
}

bool GMetaClass::FindMethod(StringID name, MetaMethod* method)
{
    method = Methods.Find(name);
    return method != nullptr;
}

void GMetaClass::AddProperty(MetaProperty* property)
{
    Properties.Add(property);
}

MetaMethod* GMetaClass::AddMethod(MetaMethod&& method)
{
    return &Methods.Add(method.GetMethodName(), std::forward<MetaMethod>(method));
}

GMetaStruct::~GMetaStruct()
{
    for (auto property : Properties)
    {
        delete property;
    }
    Properties.Clear();
}

bool GMetaStruct::IsChildOf(GMetaStruct* metaStruct) const
{
    if (this == metaStruct)
    {
        return false;
    }

    GMetaStruct* super = SuperStruct;
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

bool GMetaStruct::FindProperty(StringID name, MetaProperty*& property)
{
    int32 index = Properties.Find([&name](MetaProperty* elem){
        return elem->GetPropertyName() == name;
    });

    if (index == INDEX_NONE)
    {
        property = nullptr;
        return false;
    }

    property = Properties[index];
    return true;
}

void GMetaStruct::AddProperty(MetaProperty* property)
{
    Properties.Add(property);
}

int64 GMetaEnum::GetEnumConstantValueByIndex(int32 index) const
{
    if (ConstantDecl.IsValidIndex(index))
    {
        return ConstantDecl[index].ConstantValue;
    }

    return INDEX_NONE;
}

StringID GMetaEnum::GetEnumConstantNameByIndex(int32 index) const
{
    if (ConstantDecl.IsValidIndex(index))
    {
        return ConstantDecl[index].ConstantName;
    }

    return "";
}

#if WITH_META_DATA
String GMetaEnum::GetEnumConstantDisplayNameByIndex(int32 index) const
{
    if (!ConstantDecl.IsValidIndex(index))
    {
        return "";
    }

    const GMetaEnumConstantDecl& decl = ConstantDecl[index];

    String* displayName = decl.MetaDataMap.Find("DisplayName");
    if (displayName)
    {
        return *displayName;
    }

    return decl.ConstantName.ToString();
}
#endif

GMetaEnumConstantDecl& GMetaEnum::AddConstantDecl(GMetaEnumConstantDecl&& decl)
{
    ConstantDecl.Add(decl);
    return ConstantDecl.Last();
}