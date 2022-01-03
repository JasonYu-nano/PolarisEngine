#pragma once
//#define VK_KHR_surface
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include "core_minimal_public.hpp"

namespace Engine
{
    class VulkanWindowsHelper
    {
    public:
        static bool GetSurfaceKHR(const VkInstance& instance, VkSurfaceKHR& outSurface);
        static void GetExtraExtensions(Vector<const schar*> extensions);
    };

    typedef VulkanWindowsHelper VulkanPlatformHelper;
}