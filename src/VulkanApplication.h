#ifndef _VULKAN_APPLICATION_H_
#define _VULKAN_APPLICATION_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <array>
#include <vector>
#include <iostream>
#include <stdexcept>

#include "vk/VkDeleter.h"

class VulkanApplication
{
	private:
		GLFWwindow *window;
		VkDeleter<VkInstance> instance {vkDestroyInstance};
		
		std::vector<VkExtensionProperties> extensions;
		
		void enableValidationLayers(VkInstanceCreateInfo &createInfo);
		void createInstance();

		void initWindow();
		void initVulkan();
		void mainLoop();

	public:
		void run();

		static constexpr const char *APPLICATION_NAME = "Vulkan Test";
		
};

#endif
