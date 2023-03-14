#pragma once

#include "meta_structure.hpp"

namespace Engine
{
    class CORE_API MetaObjectRegistry
    {
        friend struct Private::MetaObjectInitializer;
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
        StructureRegister(StringID nameID) : NameID(nameID) {};

        virtual MetaClass* Register() = 0;

        StringID NameID;
    };

    template <typename ClassType>
    struct CORE_API MetaClassRegister : public StructureRegister
    {
        explicit MetaClassRegister(StringID nameID) : StructureRegister(nameID)
        {
            Private::MetaObjectInitializer::CollectDeferRegisterMetaClass(this);
        }

        MetaClass* Register() override
        {
            return ClassType::MetaObject();
        }
    };

namespace Private
{

    struct CORE_API MetaObjectInitializer
    {
        static void Initialize();

        static void CollectDeferRegisterMetaClass(StructureRegister* inst);

        static MetaClass* ConstructMetaClass(StringID className, EMetaFlag flag, Map<StringID, String>&& attributes);

        static void AddMetaProperty(MetaClass* metaClass, StringID propName, size_t offset, EMetaFlag flag, Map<StringID, String>&& attributes)
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
