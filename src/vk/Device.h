#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "VkDeleter.h"

#include <set>
#include <array>
#include <vector>

class Device
{
	private:
		friend class SwapChain;
		
		VkPhysicalDevice physicalDevice;
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;

		int presentQueueFamily = -1;
		int graphicsQueueFamily = -1;

		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkDeleter<VkDevice> device {vkDestroyDevice};

	public:
		Device(const VkPhysicalDevice &setDevice);
		
		void createLogicalDevice(const VkSurfaceKHR &surface);

		bool isDeviceSuitable() const;
		int deviceRank() const;

		VkPhysicalDevice &getVkPhysicalDevice();
		const VkPhysicalDevice &getVkPhysicalDevice() const;

		VkDevice &getVkDevice();
		const VkDevice &getVkDevice() const;

		VkQueue &getGraphicsQueue();
		const VkQueue &getGraphicsQueue() const;

		VkQueue &getPresentQueue();
		const VkQueue &getPresentQueue() const;

		void setUpSwapChainQueueFamilies(VkSwapchainCreateInfoKHR &createInfo) const;		
		bool hasSwapChainSupport(const VkSurfaceKHR &surface) const;

		static constexpr int DEVICE_TYPE_SCORE = 1000;
};

#endif
