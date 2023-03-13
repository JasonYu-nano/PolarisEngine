#pragma once

#include "meta_structure.hpp"

namespace Engine
{
    class CORE_API MetaObjectRegistry
    {
        friend class MetaObjectInitializer;
    public:
        static void Initialize();

        static MetaClass* GetMetaClassChecked(StringID className);

    private:
        static Map<StringID, MetaClass*> ClassMap;
        static Map<StringID, MetaStruct*> StructMap;
        static Map<StringID, MetaEnum*> EnumMap;
    };

    struct CORE_API StructureRegister
    {
        StructureRegister() = default;
    };

    template <typename ClassType>
    struct CORE_API MetaClassRegister : public StructureRegister
    {
        MetaClassRegister() : StructureRegister()
        {
            MetaClassRegisterCollect(this);
        }

        MetaClass* Register()
        {
            return ClassType::MetaClass();
        }
    };

    inline void MetaClassRegisterCollect(StructureRegister* registerInst)
    {
        //TODO: Defer init
    }

namespace Private
{

    struct CORE_API MetaObjectInitializer
    {
        static void Initialize()
        {
            /**
             *  1. Malloc member for meta object
             *  2. Iterator all instance, call register
             */
        }

        static MetaClass* ConstructMetaClass(const char* className, EMetaFlag flag, const Map<StringID, String>& attributes)
        {
            StringID nameID = StringID(className);
            MetaClass* metaClass = MetaObjectRegistry::GetMetaClassChecked(nameID);
            new(metaClass) MetaClass(StringID(className), flag, attributes);
            return metaClass;
        }

        static void AddMetaProperty(MetaClass* metaClass, const char* propName, size_t offset, EMetaFlag flag, const Map<StringID, String>& attributes)
        {
            metaClass->AddProperty(MetaProperty(propName, offset, flag, attributes));
        }

        static void AddMetaMethod(MetaClass* metaClass, const char* propName, size_t offset, EMetaFlag flag, const Map<StringID, String>& attributes)
        {
            metaClass->AddProperty(MetaProperty(propName, offset, flag, attributes));
        }
    };
}
}
