#pragma once
#define VK_KHR_surface
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

namespace Engine
{
    class VulkanWindowsHelper
    {
    public:
        static bool GetSurfaceKHR(const VkInstance& instance, VkSurfaceKHR& outSurface);
        static Vector<const schar*> GetRequiredExtensions();
    };

    typedef VulkanWindowsHelper VulkanPlatformHelper;
}