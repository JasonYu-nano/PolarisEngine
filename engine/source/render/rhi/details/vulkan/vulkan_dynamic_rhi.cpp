//#include "GLFW/glfw3.h"
#include "core_minimal_public.hpp"
#include "rhi/details/vulkan/vulkan_dynamic_rhi.hpp"
#include "rhi/details/vulkan/vulkan_platform.hpp"

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        PL_VERBOSE("Render", pCallbackData->pMessage)
            break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        PL_INFO("Render", pCallbackData->pMessage)
            break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        PL_WARN("Render", pCallbackData->pMessage)
            break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        PL_ERROR("Render", pCallbackData->pMessage)
            break;
    default:
        break;
    }

    return VK_FALSE;
}

namespace Engine
{
    void VulkanDynamicRHI::Init()
    {
        InitInstance();
        SetupDebugMessenger();

        if (!CreateSurface())
        {
            return;
        }
        
        if (!FindPhysicalDevice())
        {
            return;
        }

        if (!CreateLogicalDevice())
        {
            return;
        }

        if (!CheckDeviceSuitable(PhysicalDevice))
        {
            return;
        }
    }

    void VulkanDynamicRHI::Shutdown()
    {
        vkDestroyDevice(Device, nullptr);
        DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
        vkDestroySurfaceKHR(Instance, Surface, nullptr);
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

        VkArray<const ansi*> extensions = GetExtraExtensions();

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.enabledExtensionCount = extensions.GetCount();
        createInfo.ppEnabledExtensionNames = extensions.GetData();
        createInfo.pNext = nullptr;
#if VULKAN_DEBUG_MODE
        FillUpValidationSetting(createInfo);
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#endif

        if (vkCreateInstance(&createInfo, nullptr, &Instance) != VK_SUCCESS) 
        {
            PL_FATAL("Render", _T("Failed to create vulkan instance"));
            return;
        }
    }

    VkArray<const ansi*> VulkanDynamicRHI::GetExtraExtensions()
    {
        uint32 glfwExtensionCount = 0;
        /*const ansi** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);*/

        VkArray<const ansi*> extensions;
        VulkanPlatformHelper::GetExtraExtensions(extensions);
#if VULKAN_DEBUG_MODE
        extensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
        return extensions;
    }

    bool VulkanDynamicRHI::CheckValidationLayerSupport()
    {
        uint32 layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        VkArray<VkLayerProperties> availableLayers;
        availableLayers.Resize(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.GetData());

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

        return true;
    }

    void VulkanDynamicRHI::FillUpValidationSetting(VkInstanceCreateInfo& createInfo)
    {
#if VULKAN_DEBUG_MODE
        if (CheckValidationLayerSupport())
        {
            createInfo.enabledLayerCount = ValidationLayers.GetCount();
            createInfo.ppEnabledLayerNames = ValidationLayers.GetData();
        }
        else
        {
            PL_WARN("Render", _T("CheckValidationLayerSupport failed in vulkan debug mode"));
        }
#endif
    }

    void VulkanDynamicRHI::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
#if VULKAN_DEBUG_MODE
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
#endif
    }

    void VulkanDynamicRHI::SetupDebugMessenger()
    {
#if VULKAN_DEBUG_MODE
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        PopulateDebugMessengerCreateInfo(createInfo);
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr && func(Instance, &createInfo, nullptr, &DebugMessenger) == VK_SUCCESS)
        {
            return;
        }
        else
        {
            PL_ERROR("Render", _T("Failed to set up debug messenger!"));
            return;
        }
#endif
    }

    void VulkanDynamicRHI::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* allocator)
    {
#if VULKAN_DEBUG_MODE
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) 
        {
            func(instance, debugMessenger, allocator);
        }
#endif
    }

    bool VulkanDynamicRHI::FindPhysicalDevice()
    {
        uint32 deviceNum = 0;
        vkEnumeratePhysicalDevices(Instance, &deviceNum, nullptr);

        if (deviceNum <= 0)
        {
            PL_ERROR("Render", _T("Failed to find GPUs with Vulkan support"));
            return false;
        }

        VkArray<VkPhysicalDevice> devices;
        devices.Resize(deviceNum);
        vkEnumeratePhysicalDevices(Instance, &deviceNum, devices.GetData());

        for (const auto& device : devices) 
        {
            if (IsPhysicalDeviceSuitable(device)) {
                PhysicalDevice = device;
                break;
            }
        }

        return true;
    }
    bool VulkanDynamicRHI::IsPhysicalDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = FindQueueFamilies(device);
        return indices.GraphicsFamily.has_value();
    }

    QueueFamilyIndices VulkanDynamicRHI::FindQueueFamilies(VkPhysicalDevice device)
    {
        uint32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        VkArray<VkQueueFamilyProperties> queueFamilies;
        queueFamilies.Resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.GetData());

        QueueFamilyIndices indices;
        int i = 0;
        for (const auto& queueFamily : queueFamilies) 
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
            {
                indices.GraphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, Surface, &presentSupport);
            
            if (presentSupport)
            {
                indices.PresentFamily = i;
            }

            if (indices.IsValid())
            {
                break;
            }

            i++;
        }

        return indices;
    }

    bool VulkanDynamicRHI::CreateLogicalDevice()
    {
        QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice);

        VkArray<VkDeviceQueueCreateInfo> queueCreationInfos;
        Set<uint32> queueFamilies = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

        float queuePriority = 1.0f;
        for (uint32 queueFamily : queueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreationInfos.Add(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreationInfos.GetData();
        createInfo.queueCreateInfoCount = queueCreationInfos.GetCount();
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = DeviceExtensions.GetCount();
        createInfo.ppEnabledExtensionNames = DeviceExtensions.GetData();
        createInfo.enabledLayerCount = 0;

#if VULKAN_DEBUG_MODE
        createInfo.enabledLayerCount = ValidationLayers.GetCount();
        createInfo.ppEnabledLayerNames = ValidationLayers.GetData();
#endif

        if (vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &Device) != VK_SUCCESS) 
        {
            PL_ERROR("Render", _T("Failed to create logical device!"));
            return false;
        }

        vkGetDeviceQueue(Device, indices.GraphicsFamily.value(), 0, &GraphicsQueue);
        vkGetDeviceQueue(Device, indices.PresentFamily.value(), 0, &PresentQueue);

        return true;
    }

    bool VulkanDynamicRHI::CreateSurface()
    {
        return VulkanPlatformHelper::GetSurfaceKHR(Instance, Surface);
    }

    bool VulkanDynamicRHI::CheckDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = FindQueueFamilies(device);
        return indices.IsValid() && CheckDeviceExtensionSupport(device);
    }

    bool VulkanDynamicRHI::CheckDeviceExtensionSupport(VkPhysicalDevice device) const
    {
        uint32 extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        VkExtensionProperties* availableExtensions = new VkExtensionProperties[extensionCount];
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions);

        Set<std::string> requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

        for (uint32 idx = 0; idx < extensionCount; ++idx)
        {
            if (requiredExtensions.Remove(availableExtensions[idx].extensionName))
            {
                break;
            }
        }

        return requiredExtensions.IsEmpty();
    }

    SwapChainSupportDetails VulkanDynamicRHI::QuerySwapChainSupport(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;
        uint32 formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, Surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.Formats.Resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, Surface, &formatCount, details.Formats.GetData());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, Surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.PresentModes.Resize(formatCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, Surface, &presentModeCount, details.PresentModes.GetData());
        }

        return details;
    }
}
