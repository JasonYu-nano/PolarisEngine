#pragma once
#include <vulkan/vulkan.h>
#include "core_minimal_public.hpp"
#include "foundation/functional.hpp"
#include "definitions_render.hpp"

#ifdef DEBUG
#define VULKAN_DEBUG_MODE 1
#endif

namespace Engine
{
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

        VkInstance Instance;

        VkDebugUtilsMessengerEXT DebugMessenger;
        Vector<const schar*> ValidationLayers{ {"VK_LAYER_KHRONOS_validation"} };
    };
}