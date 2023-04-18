#pragma once

#define IMPL_META_STRUCT_GENERATED() \
    private: \
        static UniquePtr<class GMetaStruct> PrivateConstructMetaStruct(); \
    public: \
        static class GMetaStruct* MetaObject(); \
        \
        class GMetaStruct* GetMetaObject() const;

#define DEFINE_CONSTRUCT_META_STRUCT_START(structName) \
    GMetaStruct* structName::MetaObject() \
    { \
        static UniquePtr<GMetaStruct> meta = PrivateConstructMetaStruct(); \
        return meta.get(); \
    } \
    \
    GMetaStruct* structName::GetMetaObject() const \
    { \
        return MetaObject(); \
    } \
    \
    UniquePtr<GMetaStruct> structName::PrivateConstructMetaStruct() \
    {

#define DEFINE_CONSTRUCT_META_STRUCT_END()  return meta; }

#define DEFINE_STRUCT(name, super, flags) \
    UniquePtr<GMetaStruct> meta = MetaConstructUtils::ConstructMetaStruct(name, super, flags); \
    GMetaStruct* metaStruct = meta.get();

#if WITH_META_DATA
#define DEFINE_STRUCT_META_DATA(...) \
        MetaConstructUtils::SetMetaData(metaStruct, __VA_ARGS__);
#else
#define DEFINE_STRUCT_META_DATA(metaData)
#endif

#define DEFINE_STRUCT_PROPERTY_START(...) \
    { \
        auto* property = MetaConstructUtils::ConstructMetaProperty<GMetaStruct>(metaStruct, __VA_ARGS__);

#define DEFINE_STRUCT_PROPERTY_END() }

#define IMPL_META_CLASS_GENERATED() \
    public: \
        static class GMetaClass* MetaObject(); \
        \
        class GMetaClass* GetMetaObject() const; \
     \
    private: \
        static UniquePtr<class GMetaClass> PrivateConstructMetaClass();

#define DEFINE_CONSTRUCT_META_CLASS_START(className) \
    GMetaClass* className::MetaObject() \
    { \
        static UniquePtr<GMetaClass> meta = PrivateConstructMetaClass(); \
        return meta.get(); \
    } \
    \
    GMetaClass* className::GetMetaObject() const \
    { \
        return MetaObject(); \
    } \
    \
    UniquePtr<GMetaClass> className::PrivateConstructMetaClass() \
    {

#define DEFINE_CONSTRUCT_META_CLASS_END()  return meta; }

#define DEFINE_CLASS(...) \
    UniquePtr<GMetaClass> meta = MetaConstructUtils::ConstructMetaClass(__VA_ARGS__); \
    GMetaClass* metaClass = meta.get();

#if WITH_META_DATA
    #define DEFINE_CLASS_META_DATA(...) \
        MetaConstructUtils::SetMetaData(metaClass, __VA_ARGS__);
#else
    #define DEFINE_CLASS_META_DATA(metaData)
#endif

#define DEFINE_CLASS_PROPERTY_START(...) \
    { \
        auto* property = MetaConstructUtils::ConstructMetaProperty<GMetaClass>(metaClass, __VA_ARGS__);

#define DEFINE_CLASS_PROPERTY_END() }

#if WITH_META_DATA
    #define DEFINE_PROPERTY_META_DATA(...) \
        MetaConstructUtils::SetMetaData(property, __VA_ARGS__);
#else
    #define DEFINE_PROPERTY_META_DATA(metaData)
#endif

#define DEFINE_CLASS_METHOD_START(className, methodName, flags) \
    { \
        auto* method = MetaConstructUtils::ConstructMetaMethod(metaClass, #methodName, &className::_Execute##methodName, flags);

#define DEFINE_CLASS_METHOD_END() }

#if WITH_META_DATA
    #define DEFINE_METHOD_META_DATA(...) \
        MetaConstructUtils::SetMetaData(method, __VA_ARGS__);
#else
    #define DEFINE_METHOD_META_DATA(metaData)
#endif

#define DECLARE_PROXY_METHOD(methodName) static void _Execute##methodName(void* args);

#define DEFINE_PROXY_METHOD(className, methodName) void className::_Execute##methodName(void* args)

#define DEFINE_META_ENUM_START(enumName, flags) \
    static UniquePtr<GMetaEnum> GetMetaEnum##enumName()              \
    {                                           \
        UniquePtr<GMetaEnum> meta = MetaConstructUtils::ConstructMetaEnum(#enumName, flags); \
        GMetaEnum* metaEnum = meta.get();

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
