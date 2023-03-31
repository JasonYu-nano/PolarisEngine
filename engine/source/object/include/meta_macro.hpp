#pragma once

#define IMPL_META_STRUCT_GENERATED_START() \
    public: \
        static MetaStruct* MetaObject() \
        { \
            static UniquePtr<MetaStruct> meta = PrivateConstructMetaClass(); \
            return meta.get(); \
        }; \
     \
    private: \
        static UniquePtr<MetaStruct> PrivateConstructMetaClass() \
        { \

#define IMPL_META_STRUCT_GENERATED_END() } public:

#define DECLARE_STRUCT_START(name, super, flags) \
    UniquePtr<MetaStruct> meta = MetaConstructUtils::ConstructMetaStruct(name, super, flags); \
    MetaStruct* metaStruct = meta.get();

#define DECLARE_STRUCT_END() \
    return meta;

#if WITH_META_DATA
#define DEFINE_STRUCT_META_DATA(...) \
        MetaConstructUtils::SetMetaData(metaStruct, __VA_ARGS__);
#else
#define DEFINE_STRUCT_META_DATA(metaData)
#endif

#define DECLARE_STRUCT_PROPERTY_START(...) \
    { \
        auto&& property = MetaConstructUtils::ConstructMetaProperty<MetaStruct>(metaStruct, __VA_ARGS__);

#define DECLARE_STRUCT_PROPERTY_END() }

#define IMPL_META_CLASS_GENERATED_START() \
    public: \
        static MetaClass* MetaObject() \
        { \
            static UniquePtr<MetaClass> meta = PrivateConstructMetaClass(); \
            return meta.get(); \
        }; \
     \
    private: \
        static UniquePtr<MetaClass> PrivateConstructMetaClass() \
        { \

#define IMPL_META_CLASS_GENERATED_END() }

#define DECLARE_CLASS_START(...) \
    UniquePtr<MetaClass> meta = MetaConstructUtils::ConstructMetaClass(__VA_ARGS__); \
    MetaClass* metaClass = meta.get();

#define DECLARE_CLASS_END() \
    return meta;

#if WITH_META_DATA
    #define DEFINE_CLASS_META_DATA(...) \
        MetaConstructUtils::SetMetaData(metaClass, __VA_ARGS__);
#else
    #define DEFINE_CLASS_META_DATA(metaData)
#endif

#define DECLARE_CLASS_PROPERTY_START(...) \
    { \
        auto&& property = MetaConstructUtils::ConstructMetaProperty<MetaClass>(metaClass, __VA_ARGS__);

#define DECLARE_CLASS_PROPERTY_END() }

#if WITH_META_DATA
    #define DEFINE_PROPERTY_META_DATA(...) \
        MetaConstructUtils::SetMetaData(&property, __VA_ARGS__);
#else
    #define DEFINE_PROPERTY_META_DATA(metaData)
#endif

#define DECLARE_CLASS_METHOD_START(className, methodName, flags) \
    { \
        auto&& method = MetaConstructUtils::ConstructMetaMethod(metaClass, #methodName, &className::_Execute##methodName, flags);

#define DECLARE_CLASS_METHOD_END() }

#if WITH_META_DATA
    #define DEFINE_METHOD_META_DATA(...) \
        MetaConstructUtils::SetMetaData(&method, __VA_ARGS__);
#else
    #define DEFINE_METHOD_META_DATA(metaData)
#endif

#define DECLARE_PROXY_METHOD(methodName) static void _Execute##methodName(void* args)

#define DEFINE_META_ENUM_START(enumName, flags) \
    static UniquePtr<MetaEnum> GetMetaEnum##enumName()              \
    {                                           \
        UniquePtr<MetaEnum> meta = MetaConstructUtils::ConstructMetaEnum(#enumName, flags); \
        MetaEnum* metaEnum = meta.get();

#define DEFINE_META_ENUM_END() return meta; }

#if WITH_META_DATA
#define DEFINE_ENUM_META_DATA(...) \
        MetaConstructUtils::SetMetaData(metaEnum, __VA_ARGS__);
#else
#define DEFINE_ENUM_META_DATA(metaData)
#endif

#define DEFINE_META_CONSTANT_START(name, value, flags) \
    { \
    auto&& constant = MetaConstructUtils::ConstructEnumConstantDecl(metaEnum, #name, value, flags);

#define DEFINE_META_CONSTANT_END() }

#if WITH_META_DATA
#define DEFINE_ENUM_CONSTANT_META_DATA(...) \
        MetaConstructUtils::SetMetaData(&constant, __VA_ARGS__);
#else
#define DEFINE_ENUM_CONSTANT_META_DATA(metaData)
#endif
