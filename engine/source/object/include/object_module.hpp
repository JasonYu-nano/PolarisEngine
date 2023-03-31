#pragma once

#include "definitions_object.hpp"
#include "foundation/smart_ptr.hpp"
#include "module/module_interface.hpp"

namespace Engine
{
    class OBJECT_API ObjectModule final : public IModuleInterface
    {
        public:
        ObjectModule() = default;

        ~ObjectModule() final = default;

        void Startup() final;

        void Shutdown() final;
    };
}
