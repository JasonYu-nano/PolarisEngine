#include <vulkan/vulkan_win32.h>
#include "GLFW/glfw3.h"
#include "core_minimal_public.hpp"
#include "platform_application.hpp"
#include "rhi/details/vulkan/vulkan_windows_platform.hpp"

namespace Engine
{
    bool VulkanWindowsHelper::GetSurfaceKHR(const VkInstance& instance, VkSurfaceKHR& outSurface)
    {
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = PlatformApplication::GetHWnd();
        createInfo.hinstance = PlatformApplication::GetHInstance();

        auto func = vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");
        //if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &outSurface) != VK_SUCCESS) 
        //{
        //    //PL_ERROR("Render", TC("Failed to create window surface!"));
        //    return false;
        //}
    }

    Vector<const schar*> VulkanWindowsHelper::GetRequiredExtensions()
    {
        uint32 glfwExtensionCount = 0;
        const schar** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        Vector<const schar*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#if VULKAN_DEBUG_MODE
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
        return extensions;
    }
}