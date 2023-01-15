//#include "GLFW/glfw3.h"
#include "core_minimal_public.hpp"
#include "file_system/file_system.hpp"
#include "rhi/vulkan/vulkan_dynamic_rhi.hpp"
#include "rhi/vulkan/vulkan_platform.hpp"
#include "platform_application.hpp"

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        LOG_VERBOSE("Render", pCallbackData->pMessage)
            break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        LOG_INFO("Render", pCallbackData->pMessage)
            break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        LOG_WARN("Render", pCallbackData->pMessage)
            break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        LOG_ERROR("Render", pCallbackData->pMessage)
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

        CreateSwapChain();
        CreateImageViews();
        CreateGraphicsPipeline();
    }

    void VulkanDynamicRHI::Shutdown()
    {
        for (auto&& imageView : SwapChainImageViews)
        {
            vkDestroyImageView(Device, imageView, nullptr);
        }

        if (Device && PipelineLayout)
        {
            vkDestroyPipelineLayout(Device, PipelineLayout, nullptr);
        }

        if (Device && SwapChain)
        {
            vkDestroySwapchainKHR(Device, SwapChain, nullptr);
        }

        if (Device)
        {
            vkDestroyDevice(Device, nullptr);
        }

        if (DebugMessenger && Instance)
        {
            DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
        }

        if (Surface && Instance)
        {
            vkDestroySurfaceKHR(Instance, Surface, nullptr);
        }

        if (Instance)
        {
            vkDestroyInstance(Instance, nullptr);
        }
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

        VkArray<const char*> extensions = GetExtraExtensions();

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.enabledExtensionCount = extensions.Size();
        createInfo.ppEnabledExtensionNames = extensions.Data();
        createInfo.pNext = nullptr;
#if VULKAN_DEBUG_MODE
        FillUpValidationSetting(createInfo);
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#endif

        if (vkCreateInstance(&createInfo, nullptr, &Instance) != VK_SUCCESS) 
        {
            LOG_FATAL("Render", "Failed to create vulkan instance");
            return;
        }
    }

    VkArray<const char*> VulkanDynamicRHI::GetExtraExtensions()
    {
        uint32 glfwExtensionCount = 0;
        /*const ansi** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);*/

        VkArray<const char*> extensions;
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
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.Data());

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
            createInfo.enabledLayerCount = ValidationLayers.Size();
            createInfo.ppEnabledLayerNames = ValidationLayers.Data();
        }
        else
        {
            LOG_WARN("Render", "CheckValidationLayerSupport failed in vulkan debug mode");
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
            LOG_ERROR("Render", "Failed to set up debug messenger!");
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
            LOG_ERROR("Render", "Failed to find GPUs with Vulkan support");
            return false;
        }

        VkArray<VkPhysicalDevice> devices;
        devices.Resize(deviceNum);
        vkEnumeratePhysicalDevices(Instance, &deviceNum, devices.Data());

        for (const auto& device : devices) 
        {
            if (CheckDeviceSuitable(device))
            {
                PhysicalDevice = device;
                return true;
            }
        }

        return false;
    }

    QueueFamilyIndices VulkanDynamicRHI::FindQueueFamilies(VkPhysicalDevice device)
    {
        uint32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        VkArray<VkQueueFamilyProperties> queueFamilies;
        queueFamilies.Resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.Data());

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
        Set<uint32> queueFamilies = {indices.GraphicsFamily.value(), indices.PresentFamily.value() };

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
        createInfo.pQueueCreateInfos = queueCreationInfos.Data();
        createInfo.queueCreateInfoCount = queueCreationInfos.Size();
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = DeviceExtensions.Size();
        createInfo.ppEnabledExtensionNames = DeviceExtensions.Data();
        createInfo.enabledLayerCount = 0;

#if VULKAN_DEBUG_MODE
        createInfo.enabledLayerCount = ValidationLayers.Size();
        createInfo.ppEnabledLayerNames = ValidationLayers.Data();
#endif

        if (vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &Device) != VK_SUCCESS) 
        {
            LOG_ERROR("Render", "Failed to create logical device!");
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

        bool swapChainAdequate = false;
        if (CheckDeviceExtensionSupport(device))
        {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.Formats.Empty() && !swapChainSupport.PresentModes.Empty();
        }
        return swapChainAdequate && indices.IsValid();
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

        return requiredExtensions.Empty();
    }

    SwapChainSupportDetails VulkanDynamicRHI::QuerySwapChainSupport(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, Surface, &details.Capabilities);

        uint32 formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, Surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.Formats.Resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, Surface, &formatCount, details.Formats.Data());
        }

        uint32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, Surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.PresentModes.Resize(formatCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, Surface, &presentModeCount, details.PresentModes.Data());
        }

        return details;
    }

    VkSurfaceFormatKHR VulkanDynamicRHI::ChooseSwapSurfaceFormat(const VkArray<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VulkanDynamicRHI::ChooseSwapPresentMode(const VkArray<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanDynamicRHI::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != MAX_UINT32)
        {
            return capabilities.currentExtent;
        }
        else
        {
            int32 width, height;
            PlatformApplication::GetFrameBufferSize(width, height);

            VkExtent2D actualExtent =
            {
                static_cast<uint32>(width),
                static_cast<uint32>(height)
            };

            actualExtent.width = Math::Clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = Math::Clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void VulkanDynamicRHI::CreateSwapChain()
    {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(PhysicalDevice);

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.Formats);
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.PresentModes);
        VkExtent2D extent = ChooseSwapExtent(swapChainSupport.Capabilities);

        uint32 imageCount = swapChainSupport.Capabilities.minImageCount + 1;
        if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = Surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice);
        uint32 queueFamilyIndices[] = {indices.GraphicsFamily.value(), indices.PresentFamily.value()};

        if (indices.GraphicsFamily != indices.PresentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }
        createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(Device, &createInfo, nullptr, &SwapChain) != VK_SUCCESS)
        {
            LOG_ERROR("Render", "Failed to create swap chain!");
            return;
        }

        vkGetSwapchainImagesKHR(Device, SwapChain, &imageCount, nullptr);
        SwapChainImages.Resize(imageCount);
        vkGetSwapchainImagesKHR(Device, SwapChain, &imageCount, SwapChainImages.Data());

        SwapChainImageFormat = surfaceFormat.format;
        SwapChainExtent = extent;
    }

    void VulkanDynamicRHI::CreateImageViews()
    {
        SwapChainImageViews.Resize(SwapChainImages.Size());
        uint32 idx = 0;
        for (const auto& image : SwapChainImages)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = image;
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = SwapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(Device, &createInfo, nullptr, &SwapChainImageViews[idx]) != VK_SUCCESS)
            {
                LOG_ERROR("", "Failed to create image views!");
            }
            ++idx;
        }
    }

    void VulkanDynamicRHI::CreateGraphicsPipeline()
    {
        Array64<uint8> vertShader;
        Array64<uint8> fragShader;
        FileSystem::ReadFileToBinary(FileSystem::GetEngineRootPath() / "intermediate/generated/shader/spv/shader.vert.spv", vertShader);
        FileSystem::ReadFileToBinary(FileSystem::GetEngineRootPath() / "intermediate/generated/shader/spv/shader.frag.spv", fragShader);

        VkShaderModule vertShaderModule = CreateShaderModule(vertShader);
        VkShaderModule fragShaderModule = CreateShaderModule(fragShader);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        VkPipelineVertexInputStateCreateInfo vertInputInfo = {};
        vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertInputInfo.vertexBindingDescriptionCount = 0;
        vertInputInfo.vertexAttributeDescriptionCount = 0;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(SwapChainExtent.width);
        viewport.height = static_cast<float>(SwapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = SwapChainExtent;

        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        if (vkCreatePipelineLayout(Device, &pipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
        {
            LOG_ERROR("Render", "Failed to create pipeline layout");
        }


        vkDestroyShaderModule(Device, vertShaderModule, nullptr);
        vkDestroyShaderModule(Device, fragShaderModule, nullptr);
    }

    VkShaderModule VulkanDynamicRHI::CreateShaderModule(Array64<uint8> code)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.Size();
        createInfo.pCode = reinterpret_cast<const uint32*>(code.Data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            LOG_ERROR("Render", "Create shader module failed");
        }

        return shaderModule;
    }
}
