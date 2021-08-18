#pragma once

#include "definitions_render.hpp"
#include "rhi/details/vulkan/vulkan_dynamic_rhi.hpp"
#include "foundation/smart_ptr.hpp"

namespace Engine
{
    class RENDER_API RenderModule
    {
    public:
        static RenderModule& Get();

        void Init();

        void Shutdown();

    private:
        RenderModule() = default;

        UniquePtr<VulkanDynamicRHI> RHI;
    };
}