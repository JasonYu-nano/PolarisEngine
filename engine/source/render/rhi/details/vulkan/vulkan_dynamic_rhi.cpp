#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"
#include "core_minimal_public.hpp"
#include "rhi/details/vulkan/vulkan_dynamic_rhi.hpp"

namespace Engine
{
    void VulkanDynamicRHI::Init()
    {

    }

    void VulkanDynamicRHI::Shutdown()
    {
        vkDestroyInstance(Instance, nullptr);
    }

    void VulkanDynamicRHI::InitInstance()
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Polaris";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Polaris Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        uint32 glfwExtensionCount = 0;
        const schar** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        if (vkCreateInstance(&createInfo, nullptr, &Instance) != VK_SUCCESS) 
        {
            PL_FATAL(TC("Failed to create vulkan instance"));
            return;
        }
    }
}
