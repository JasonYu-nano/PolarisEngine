#pragma once
#include <vulkan/vulkan.h>
#include "core_minimal_public.hpp"
#include "foundation/functional.hpp"
#include "foundation/stl_misc.hpp"
#include "definitions_render.hpp"
#include "rhi/details/vulkan/vulkan_type.hpp"

#ifdef DEBUG
#define VULKAN_DEBUG_MODE 1
#endif

namespace Engine
{
    struct QueueFamilyIndices
    {
        Optional<uint32> GraphicsFamily;
        Optional<uint32> PresentFamily;

        bool IsValid() const
        {
            return GraphicsFamily.has_value() && PresentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        VkArray<VkSurfaceFormatKHR> Formats;
        VkArray<VkPresentModeKHR> PresentModes;
    };

    class RENDER_API VulkanDynamicRHI
    {
    public:
        void Init();

        void Shutdown();

    private:
        void InitInstance();

        VkArray<const ansi*> GetExtraExtensions();

        bool CheckValidationLayerSupport();

        void FillUpValidationSetting(VkInstanceCreateInfo& createInfo);

        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        void SetupDebugMessenger();

        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* allocator);

        bool FindPhysicalDevice();

        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

        bool CreateLogicalDevice();

        bool CreateSurface();

        bool CheckDeviceSuitable(VkPhysicalDevice device);

        bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;

        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const VkArray<VkSurfaceFormatKHR>& availableFormats);

        VkPresentModeKHR ChooseSwapPresentMode(const VkArray<VkPresentModeKHR>& availablePresentModes);

        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        void CreateSwapChain();

        void CreateImageViews();

        void CreateGraphicsPipeline();

        VkShaderModule CreateShaderModule(DynamicArray64<uint8> code);
    private:
        VkInstance Instance{ nullptr };

        VkDebugUtilsMessengerEXT DebugMessenger{ nullptr };
        VkArray<const ansi*> ValidationLayers{ "VK_LAYER_KHRONOS_validation" };
        VkArray<const ansi*> DeviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        VkPhysicalDevice PhysicalDevice{ nullptr };
        VkDevice Device{ nullptr };
        VkQueue GraphicsQueue{ nullptr };
        VkQueue PresentQueue{ nullptr };
        VkSurfaceKHR Surface{ nullptr };
        VkSwapchainKHR SwapChain{ nullptr };

        VkArray<VkImage> SwapChainImages;
        VkFormat SwapChainImageFormat;
        VkExtent2D SwapChainExtent;

        VkPipelineLayout PipelineLayout;

        VkArray<VkImageView> SwapChainImageViews;
    };
}