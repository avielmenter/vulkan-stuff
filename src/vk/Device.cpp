#include "Device.h"

#include <string.h>

constexpr std::array<const char*, 1> REQUIRED_DEVICE_EXTENSIONS =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

Device::Device(const VkPhysicalDevice &setDevice) : physicalDevice(setDevice)
{
	vkGetPhysicalDeviceProperties(this->physicalDevice, &this->properties);
	vkGetPhysicalDeviceFeatures(this->physicalDevice, &this->features);
}

void Device::createLogicalDevice(const VkSurfaceKHR &surface)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice, &queueFamilyCount, queueFamilies.data());	

	int graphicsQueueFamily = -1;
	int presentQueueFamily = -1;
	int i = 0;

	for (const auto &queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			graphicsQueueFamily = i;
		
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(this->physicalDevice, i, surface, &presentSupport); 
		if (queueFamily.queueCount > 0 && presentSupport)
			presentQueueFamily = i;

		i++;
	}

	if (graphicsQueueFamily == -1 || presentQueueFamily == -1)
		throw std::runtime_error("Failed to find suitable graphics or presentation queues.");
	
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = { graphicsQueueFamily, presentQueueFamily };

	for (int queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo;
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures featuresDummy;

	VkDeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pEnabledFeatures = &featuresDummy;
	deviceCreateInfo.enabledExtensionCount = REQUIRED_DEVICE_EXTENSIONS.size();
	deviceCreateInfo.ppEnabledExtensionNames = REQUIRED_DEVICE_EXTENSIONS.data();
	deviceCreateInfo.enabledLayerCount = 0;

	if (vkCreateDevice(this->physicalDevice, &deviceCreateInfo, nullptr, this->device.replace()) != VK_SUCCESS)
		throw std::runtime_error("Failed to create logical device.");

	vkGetDeviceQueue(this->device.get(), graphicsQueueFamily, 0, &this->graphicsQueue);	
	if (graphicsQueueFamily != presentQueueFamily)
		vkGetDeviceQueue(this->device.get(), presentQueueFamily, 0, &this->presentQueue);

	this->sameGraphicsAndPresentQueue = graphicsQueueFamily == presentQueueFamily;
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

	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(this->physicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(this->physicalDevice, nullptr, &extensionCount, availableExtensions.data());

	for (const char *extensionName : REQUIRED_DEVICE_EXTENSIONS)
	{
		bool extensionFound = false;

		for (const auto &availableExtension : availableExtensions)
		{
			if (strcmp(availableExtension.extensionName, extensionName) == 0)
			{
				extensionFound = true;
				break;
			}
		}

		if (!extensionFound)
			return 0;
	}

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

VkQueue &Device::getGraphicsQueue()
{
	return this->graphicsQueue;
}

const VkQueue &Device::getGraphicsQueue() const
{
	return this->graphicsQueue;
}

VkQueue &Device::getPresentQueue()
{
	if (!this->sameGraphicsAndPresentQueue)
		return this->presentQueue;
	return this->graphicsQueue;
}

const VkQueue &Device::getPresentQueue() const
{
	if (!this->sameGraphicsAndPresentQueue)
		return this->presentQueue;
	return this->graphicsQueue;
}
