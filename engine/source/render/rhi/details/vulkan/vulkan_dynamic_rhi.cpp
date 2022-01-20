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
        PL_INFO("Render", pCallbackData->pMessage)
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

        Vector<const ansi*> extensions = GetExtraExtensions();

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.enabledExtensionCount = static_cast<uint32>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
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

    Vector<const ansi*> VulkanDynamicRHI::GetExtraExtensions()
    {
        uint32 glfwExtensionCount = 0;
        /*const ansi** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);*/

        Vector<const ansi*> extensions;
        VulkanPlatformHelper::GetExtraExtensions(extensions);
#if VULKAN_DEBUG_MODE
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
        return extensions;
    }

    bool VulkanDynamicRHI::CheckValidationLayerSupport()
    {
        uint32 layerCount = 0;
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

        return true;
    }

    void VulkanDynamicRHI::FillUpValidationSetting(VkInstanceCreateInfo& createInfo)
    {
#if VULKAN_DEBUG_MODE
        if (CheckValidationLayerSupport())
        {
            createInfo.enabledLayerCount = static_cast<uint32>(ValidationLayers.size());
            createInfo.ppEnabledLayerNames = ValidationLayers.data();
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

        Vector<VkPhysicalDevice> devices(deviceNum);
        vkEnumeratePhysicalDevices(Instance, &deviceNum, devices.data());

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

        Vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        QueueFamilyIndices indices;
        int i = 0;
        for (const auto& queueFamily : queueFamilies) 
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
            {
                indices.GraphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, Surface, &presentSupport);
            
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

        Vector<VkDeviceQueueCreateInfo> queueCreationInfos;
        Set<uint32> queueFamilies = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

        float queuePriority = 1.0f;
        for (uint32 queueFamily : queueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreationInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreationInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32>(queueCreationInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = 0;
        createInfo.enabledLayerCount = 0;

#if VULKAN_DEBUG_MODE
        createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
        createInfo.ppEnabledLayerNames = ValidationLayers.data();
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
}
