#pragma once


#define DECLARE_CLASS_START(class, flag, attributes) \
    MetaClass* metaClass = Engine::Private::MetaObjectInitializer::ConstructMetaClass(#class, flag, attributes);

#define DECLARE_CLASS_END() return metaClass;

#define DECLARE_CLASS_PROPERTY(prop, offset, flag, attributes) \
    Engine::Private::MetaObjectInitializer::AddMetaProperty(metaClass, #prop, offset, flag, attributes);