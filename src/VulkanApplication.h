#ifndef _VULKAN_APPLICATION_H_
#define _VULKAN_APPLICATION_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <array>
#include <vector>
#include <iostream>
#include <stdexcept>

#include "vk/Device.h"

#ifdef DEBUG
VkResult createDebugReportCallback(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugReportCallbackEXT *pCallback)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pCallback);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void destroyDebugReportCallback(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks *pAllocator)
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (func != nullptr)
		func(instance, callback, pAllocator);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL validationCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char *layerPrefix, const char *msg, void *userdata)
{
	std::cerr << "VALIDATION: " << msg << std::endl;
	return VK_FALSE;
}
#endif

class VulkanApplication
{
	private:
		GLFWwindow *window;
		VkDeleter<VkInstance> instance {vkDestroyInstance};
		VkDeleter<VkSurfaceKHR> surface {this->instance, vkDestroySurfaceKHR};

		std::vector<VkExtensionProperties> extensions;
		std::vector<Device> devices;

		Device *currentDevice = nullptr;

		#ifdef DEBUG
		VkDeleter<VkDebugReportCallbackEXT> callback{instance, destroyDebugReportCallback};
		void enableValidationLayers(VkInstanceCreateInfo &createInfo);
		#endif
	
		void createInstance();
		void findPhysicalDevices();

		void initWindow();
		void initVulkan();
		void createSurface();
		void pickPhysicalDevice();
		void mainLoop();

	public:
		void run();

		static constexpr const char *APPLICATION_NAME = "Vulkan Test";
		
};

#endif
