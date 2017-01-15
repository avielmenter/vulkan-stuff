#include "Device.h"

Device::Device(const VkPhysicalDevice &setDevice) : physicalDevice(setDevice)
{
	vkGetPhysicalDeviceProperties(this->physicalDevice, &this->properties);
	vkGetPhysicalDeviceFeatures(this->physicalDevice, &this->features);
}

bool Device::isQueueSuitable(const VkQueueFamilyProperties &queue)
{
	return queue.queueCount > 0 && queue.queueFlags & VK_QUEUE_GRAPHICS_BIT;
}

void Device::createLogicalDevice()
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice, &queueFamilyCount, queueFamilies.data());	

	VkDeviceQueueCreateInfo queueCreateInfo;
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

	int i;
	for (i = 0; !this->isQueueSuitable(queueFamilies[i]) && i < queueFamilyCount; i++);
	
	queueCreateInfo.queueFamilyIndex = i;
	queueCreateInfo.queueCount = 1;
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures featuresDummy;

	VkDeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pEnabledFeatures = &featuresDummy;
	deviceCreateInfo.enabledExtensionCount = 0;
	deviceCreateInfo.enabledLayerCount = 0;

	if (vkCreateDevice(this->physicalDevice, &deviceCreateInfo, nullptr, this->device.replace()) != VK_SUCCESS)
		throw std::runtime_error("Failed to create logical device.");

	vkGetDeviceQueue(this->device, queueCreateInfo.queueFamilyIndex, 0, &this->queue);	
}

bool Device::isDeviceSuitable() const
{
	return this->deviceRank() > 0;
}

int Device::deviceRank() const
{
	int score = 0;

	if (this->properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		score += Device::DEVICE_TYPE_SCORE;
	score += this->properties.limits.maxImageDimension2D;

	if (this->features.geometryShader)
		return 0;

	return score;
}

VkPhysicalDevice &Device::getVkPhysicalDevice()
{
	return this->physicalDevice;
}

const VkPhysicalDevice &Device::getVkPhysicalDevice() const
{
	return this->physicalDevice;
}

VkDevice &Device::getVkDevice()
{
	return this->device.get();
}

const VkDevice &Device::getVkDevice() const
{
	return this->device.get();
}
