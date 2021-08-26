/*
#include "app/details/vulkan/vulkan_app_driver.hpp"

namespace Engine
{
   void VulkanAppDriver::Init()
   {
       glfwInit();
       glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
       glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
       Window = GLWindowUniquePtr(glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr), [](GLFWwindow* ptr){
           glfwDestroyWindow(ptr);
       });
   }

   void VulkanAppDriver::Shutdown()
   {
       glfwTerminate();
   }

   void VulkanAppDriver::Tick()
   {
       while (!glfwWindowShouldClose(Window.get()))
       {
           glfwPollEvents();
       }
   }
}
*/