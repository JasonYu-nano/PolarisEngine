#pragma once

#include "meta_structure.hpp"

namespace Engine
{
    class CORE_API MetaObjectRegistry
    {
        friend class MetaObjectInitializer;
    public:
        static void Initialize();

        static void Finalize();

        static MetaClass* GetMetaClassChecked(StringID className);

    private:
        static Map<StringID, MetaClass*> ClassMap;
        static Map<StringID, MetaStruct*> StructMap;
        static Map<StringID, MetaEnum*> EnumMap;
    };

    struct CORE_API StructureRegister
    {
        StructureRegister() = default;

        virtual MetaClass* Register() = 0;
    };

    template <typename ClassType>
    struct CORE_API MetaClassRegister : public StructureRegister
    {
        MetaClassRegister() : StructureRegister()
        {
            Private::MetaObjectInitializer::CollectDeferRegisterMetaClass(this);
        }

        MetaClass* Register() override
        {
            return ClassType::MetaClass();
        }
    };

namespace Private
{

    struct CORE_API MetaObjectInitializer
    {
        static void Initialize();

        static void CollectDeferRegisterMetaClass(StructureRegister* inst);

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

        static Array<StructureRegister*> DeferRegisterMetaClassArray;
    };
}
}
