#include "app/details/app_driver.hpp"
#include "app/details/vulkan/vulkan_app_driver.hpp"

namespace Engine
{
    UniquePtr<IAppDriver> AppDriverHelper::GetDriver()
    {
        return MakeUniquePtr<VulkanAppDriver>();
    }
}