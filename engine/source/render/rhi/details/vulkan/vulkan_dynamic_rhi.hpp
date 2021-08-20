#pragma once
#include <vulkan/vulkan.h>
#include "definitions_render.hpp"
#include "foundation/functional.hpp"
#include "foundation/array.hpp"
#include "foundation/string.hpp"

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
#if VULKAN_DEBUG_MODE
        bool CheckValidationLayerSupport();

        void FillUpValidationSetting(VkInstanceCreateInfo& createInfo);
#endif

        VkInstance Instance;

#if VULKAN_DEBUG_MODE
        Vector<const schar*> ValidationLayers{ {"VK_LAYER_KHRONOS_validation"} };
#endif
    };
}