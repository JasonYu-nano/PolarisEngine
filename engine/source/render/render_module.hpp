#pragma once

#include "definitions_render.hpp"
#include "foundation/smart_ptr.hpp"
#include "module/module_interface.hpp"
#include "rhi/details/vulkan/vulkan_dynamic_rhi.hpp"

namespace Engine
{
    class RENDER_API RenderModule final : public IModuleInterface
    {
    public:
        RenderModule() = default;

        ~RenderModule() final = default;

        void Startup() final;

        void Shutdown() final;

    private:
        UniquePtr<VulkanDynamicRHI> RHI;
    };
}