#pragma once

#include "definitions_object.hpp"
#include "meta_types.hpp"
#include "foundation/string_id.hpp"
#include "object.gen.hpp"

namespace Engine
{
    class OBJECT_API META() GObject
    {
        MOC_GENERATED()
    public:
        virtual ~GObject() = default;

        META()
        bool IsA(class GMetaClass* metaClass) const;

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
