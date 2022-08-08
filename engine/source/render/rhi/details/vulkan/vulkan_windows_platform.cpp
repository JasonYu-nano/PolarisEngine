#include "rhi/details/vulkan/vulkan_windows_platform.hpp"
#include <vulkan/vulkan_win32.h>
#include "core_minimal_public.hpp"
#include "platform_application.hpp"

namespace Engine
{
    bool VulkanWindowsHelper::GetSurfaceKHR(const VkInstance& instance, VkSurfaceKHR& outSurface)
    {
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = PlatformApplication::GetHWnd();
        createInfo.hinstance = PlatformApplication::GetHInstance();

        auto func = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");
        if (func != nullptr && func(instance, &createInfo, nullptr, &outSurface) == VK_SUCCESS)
        {
            return true;
        }
        LOG_ERROR("Render", "Failed to create window surface!");
        return false;
    }

    void VulkanWindowsHelper::GetExtraExtensions(VkArray<const ansi*>& extensions)
    {
        extensions.Add(VK_KHR_SURFACE_EXTENSION_NAME);
        extensions.Add(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    }
}