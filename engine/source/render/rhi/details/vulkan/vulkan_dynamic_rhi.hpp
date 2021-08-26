#pragma once
#include <vulkan/vulkan.h>
#include "core_minimal_public.hpp"
#include "foundation/functional.hpp"
#include "foundation/stl_misc.hpp"
#include "definitions_render.hpp"

#ifdef DEBUG
#define VULKAN_DEBUG_MODE 1
#endif

namespace Engine
{
    struct QueueFamilyIndices
    {
        Optional<uint32> GraphicsFamily;
    };

    class RENDER_API VulkanDynamicRHI
    {
        /*template<typename Type>
        using VKUniquePtr = UniquePtr<Type, TFunction<void(Type*)>>;*/

    public:
        void Init();

        void Shutdown();

    private:
        void InitInstance();

        Vector<const schar*> GetRequiredExtensions();

        bool CheckValidationLayerSupport();

        void FillUpValidationSetting(VkInstanceCreateInfo& createInfo);

        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        void SetupDebugMessenger();

        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* allocator);

        bool FindPhysicalDevice();

        bool IsPhysicalDeviceSuitable(VkPhysicalDevice device);

        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

        bool CreateLogicalDevice();

        bool CreateSurface();
    private:
        VkInstance Instance;

        VkDebugUtilsMessengerEXT DebugMessenger;
        Vector<const schar*> ValidationLayers{ {"VK_LAYER_KHRONOS_validation"} };

        VkPhysicalDevice PhysicalDevice{ nullptr };
        VkDevice Device{ nullptr };
        VkQueue GraphicsQueue{ nullptr };
        VkSurfaceKHR surface{ nullptr };
    };
}