#include "GLFW/glfw3.h"
#include "core_minimal_public.hpp"
#include "rhi/details/vulkan/vulkan_dynamic_rhi.hpp"

namespace Engine
{
    void VulkanDynamicRHI::Init()
    {
        InitInstance();
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
#if VULKAN_DEBUG_MODE
        FillUpValidationSetting(createInfo);
#endif

        if (vkCreateInstance(&createInfo, nullptr, &Instance) != VK_SUCCESS) 
        {
            PL_FATAL(TC("Failed to create vulkan instance"));
            return;
        }
    }

#if VULKAN_DEBUG_MODE
    bool VulkanDynamicRHI::CheckValidationLayerSupport()
    {
        uint32 layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        Vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : ValidationLayers) 
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) 
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) 
            {
                return false;
            }
        }

        return false;
    }

    void VulkanDynamicRHI::FillUpValidationSetting(VkInstanceCreateInfo& createInfo)
    {
        if (CheckValidationLayerSupport())
        {
            createInfo.enabledLayerCount = static_cast<uint32>(ValidationLayers.size());
            createInfo.ppEnabledLayerNames = ValidationLayers.data();
        }
        else
        {
            PL_WARN(TC("CheckValidationLayerSupport failed in vulkan debug mode"));
        }
    }
#endif
}
