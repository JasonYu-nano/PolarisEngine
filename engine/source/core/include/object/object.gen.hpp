#pragma once

#include "mos/meta_register.hpp"
#include "mos/meta_gen_macro.hpp"

namespace Engine
{
#define META_CLASS_GENERATED_GObject() \
    IMPL_META_CLASS_GENERATED_START()  \
        DECLARE_CLASS_START("GObject", EMetaFlag::MF_None, Map<StringID, String>()) \
            DECLARE_CLASS_PROPERTY("ID", OFFSET_OF(GObject, ID), EMetaFlag::MF_None, Map<StringID, String>()) \
        DECLARE_CLASS_END() \
    IMPL_META_CLASS_GENERATED_END()

    class GObject;
    static MetaClassRegister<GObject> _register("GObject");
}
