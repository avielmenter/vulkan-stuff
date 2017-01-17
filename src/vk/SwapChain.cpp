#include "SwapChain.h"

#include <algorithm>
#include <limits>

SwapChain::SwapChain(const Device &d, const VkSurfaceKHR &s) : device(d), surface(s)
{	
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->device.getVkPhysicalDevice(), this->surface, &this->capabilities);

	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(this->device.getVkPhysicalDevice(), this->surface, &formatCount, nullptr);

	if (formatCount > 0)
	{
		this->formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(this->device.getVkPhysicalDevice(), this->surface, &formatCount, this->formats.data());
	}

	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(this->device.getVkPhysicalDevice(), this->surface, &presentModeCount, nullptr);

	if (presentModeCount > 0)
	{
		this->presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(this->device.getVkPhysicalDevice(), this->surface, &presentModeCount, nullptr);
	}
}

bool SwapChain::isSwapChainSupport() const
{
	return !this->formats.empty() && !this->presentModes.empty();
}

void SwapChain::createSwapChain()
{
	this->createSwapChain(0, 0);
}

void SwapChain::createSwapChain(uint32_t setWidth, uint32_t setHeight)
{
	VkSurfaceFormatKHR format = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
	if (this->formats.size() != 1 || this->formats[0].format != VK_FORMAT_UNDEFINED)
	{
		bool preferredFormatExists = false;

		for (const auto &format : this->formats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				preferredFormatExists = true;
				break;
			}
		}

		if (!preferredFormatExists)
			format = this->formats[0];
	}

	VkPresentModeKHR presentMode = std::find(this->presentModes.begin(), this->presentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != this->presentModes.end() ? 
			VK_PRESENT_MODE_MAILBOX_KHR :
			VK_PRESENT_MODE_FIFO_KHR;
	
	VkExtent2D extent = this->capabilities.currentExtent;
	if (extent.width == std::numeric_limits<uint32_t>::max())
	{
		extent.width = std::max(this->capabilities.minImageExtent.width, std::min(this->capabilities.maxImageExtent.width, setWidth));
		extent.height = std::max(this->capabilities.maxImageExtent.height, std::min(this->capabilities.maxImageExtent.height, setHeight));
	}

	uint32_t numImages = this->capabilities.minImageCount + 1;
	if (this->capabilities.maxImageCount > 0 && numImages > this->capabilities.maxImageCount)
		numImages = this->capabilities.maxImageCount;

	this->createSwapChain(format, presentMode, extent, numImages);
}

void SwapChain::createSwapChain(const VkSurfaceFormatKHR &format, const VkPresentModeKHR &presentMode, const VkExtent2D &extent, uint32_t numImages)
{
	VkSwapchainCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = this->surface;
	createInfo.minImageCount = numImages;
	createInfo.imageFormat = format.format;
	createInfo.imageColorSpace = format.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	this->device.setUpSwapChainQueueFamilies(createInfo);
	createInfo.preTransform = this->capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(this->device.getVkDevice(), &createInfo, nullptr, this->swapChain.replace()) != VK_SUCCESS)
		throw std::runtime_error("Failed to create swap chain.");

	if (createInfo.pQueueFamilyIndices != nullptr)
		delete [] createInfo.pQueueFamilyIndices;
}
