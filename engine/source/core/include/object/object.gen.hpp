#pragma once

#include "mos/meta_register.hpp"

namespace Engine
{
#define META_CLASS_GENERATED_GObject()

    class GObject;
    static MetaClassRegister<GObject> _register;
}
