#ifndef _SWAP_CHAIN_H_
#define _SWAP_CHAIN_H_

#include "Device.h"

class SwapChain
{
	private:
		const Device &device;
		const VkSurfaceKHR &surface;

		VkDeleter<VkSwapchainKHR> swapChain { this->device.device, vkDestroySwapchainKHR };

		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;

	public:
		SwapChain(const Device &setDevice, const VkSurfaceKHR &setSurface);
		
		bool isSwapChainSupport() const;

		void createSwapChain();
		void createSwapChain(uint32_t setWidth, uint32_t setHeight);
		void createSwapChain(const VkSurfaceFormatKHR &format, const VkPresentModeKHR &presentMode, const VkExtent2D &extent, uint32_t numImages);
};

#endif
