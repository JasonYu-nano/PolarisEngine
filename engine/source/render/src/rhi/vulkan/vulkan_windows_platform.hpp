#pragma once
//#define VK_KHR_surface
#define VK_USE_PLATFORM_WIN32_KHR
#include "core_minimal_public.hpp"
#include "rhi/vulkan/vulkan_type.hpp"
#include <vulkan/vulkan.h>

namespace Engine
{
    class VulkanWindowsHelper
    {
    public:
        static bool GetSurfaceKHR(const VkInstance& instance, VkSurfaceKHR& outSurface);
        static void GetExtraExtensions(VkArray<const char*>& extensions);
    };

    typedef VulkanWindowsHelper VulkanPlatformHelper;
}