#ifndef _PHYSICAL_DEVICE_H_
#define _PHYSICAL_DEVICE_H_

#include <vulkan/vulkan.h>

class PhysicalDevice
{
	private:
		VkPhysicalDevice device;
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;

	public:
		PhysicalDevice(const VkPhysicalDevice &setDevice);

		bool isDeviceSuitable() const;
		int deviceRank() const;

		static constexpr int DEVICE_TYPE_SCORE = 1000;
};

#endif
