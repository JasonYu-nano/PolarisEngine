#pragma once

#define IMPL_META_CLASS_GENERATED_START() \
    public: \
        static MetaClass* MetaObject() \
        { \
            static MetaClass* meta = ConstructMetaClass(); \
            return meta; \
        }; \
     \
    private: \
        static MetaClass* ConstructMetaClass() \
        { \

#define IMPL_META_CLASS_GENERATED_END() }

#define DECLARE_CLASS_START(...) \
    MetaClass* metaClass = Private::MetaObjectInitializer::ConstructMetaClass(__VA_ARGS__);

#define DECLARE_CLASS_END() return metaClass;

#define DECLARE_CLASS_PROPERTY(...) \
    Private::MetaObjectInitializer::AddMetaProperty(metaClass, __VA_ARGS__);

#define DECLARE_PROXY_METHOD(methodName) static void _Execute##methodName(void* args)
