#include "render_module.hpp"

namespace Engine
{
    RenderModule& RenderModule::Get()
    {
        static RenderModule module;
        return module;
    }

    void RenderModule::Init()
    {
        RHI = MakeUniquePtr<VulkanDynamicRHI>();
        RHI->Init();
    }

    void RenderModule::Shutdown()
    {
        RHI->Shutdown();
    }
}