#pragma once

#include "definitions_object.hpp"
#include "meta_types.hpp"
#include "foundation/string_id.hpp"
#include "object.gen.hpp"

namespace Engine
{
    class OBJECT_API META() GObject
    {
        MOC_GENERATED(GObject)
    public:

        META()
        bool IsA(MetaClass* metaClass)
        {
            MetaClass* myMeta = MetaObject();
            return myMeta == metaClass or myMeta->IsChildOf(metaClass);
        }

    protected:
        META()
        StringID Name;
    };

    template <typename T>
    T* ObjectCast(GObject* object)
    {
        if (object->IsA(T::MetaObject()))
        {
            return static_cast<T*>(object);
        }
        return nullptr;
    }
}
