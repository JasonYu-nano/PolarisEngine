#include "render_module.hpp"

namespace Engine
{
    void RenderModule::Startup()
    {
        RHI = MakeUniquePtr<VulkanDynamicRHI>();
        RHI->Init();
    }

    void RenderModule::Shutdown()
    {
        RHI->Shutdown();
    }
}