#include "PhysicalDevice.h"

PhysicalDevice::PhysicalDevice(const VkPhysicalDevice &setDevice) : device(setDevice)
{
	vkGetPhysicalDeviceProperties(this->device, &this->properties);
	vkGetPhysicalDeviceFeatures(this->device, &this->features);
}

bool PhysicalDevice::isDeviceSuitable() const
{
	return this->deviceRank() > 0;
}

int PhysicalDevice::deviceRank() const
{
	int score = 0;

	if (this->properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		score += PhysicalDevice::DEVICE_TYPE_SCORE;
	score += this->properties.limits.maxImageDimension2D;

	if (this->features.geometryShader)
		return 0;

	return score;
}
