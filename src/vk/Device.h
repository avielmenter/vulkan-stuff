#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "VkDeleter.h"
#include <vector>

class Device
{
	private:
		VkPhysicalDevice physicalDevice;
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;

		VkQueue queue;
		VkDeleter<VkDevice> device {vkDestroyDevice};

		bool isQueueSuitable(const VkQueueFamilyProperties &queue);

	public:
		Device(const VkPhysicalDevice &setDevice);
		
		void createLogicalDevice();

		bool isDeviceSuitable() const;
		int deviceRank() const;

		VkPhysicalDevice &getVkPhysicalDevice();
		const VkPhysicalDevice &getVkPhysicalDevice() const;

		VkDevice &getVkDevice();
		const VkDevice &getVkDevice() const;

		VkQueue &getQueue();
		const VkQueue &getQueue() const;

		static constexpr int DEVICE_TYPE_SCORE = 1000;
};

#endif
