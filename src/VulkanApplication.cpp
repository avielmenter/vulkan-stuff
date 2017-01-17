#include "VulkanApplication.h"

#include <cstring>

constexpr const std::array<const char *, 1> VALIDATION_LAYERS =
{
	"" //"VK_LAYER_LUNARG_standard_validation"
};

VulkanApplication::~VulkanApplication()
{
	if (this->currentSwapChain != nullptr && this->currentDevice != nullptr)
		delete this->currentSwapChain;
}

#ifdef DEBUG
void VulkanApplication::enableValidationLayers(VkInstanceCreateInfo &createInfo)
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	std::vector<const char *> layersNotFound;
	for (const char *requestedLayer : VALIDATION_LAYERS)
	{
		if (requestedLayer[0] == '\0') continue;
		bool layerFound = false;

		for (const auto &availableLayer : availableLayers)
		{
			if (std::strcmp(requestedLayer, availableLayer.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
			layersNotFound.push_back(requestedLayer);
	}

	std::cout << availableLayers.size() << " available validation layers: " << std::endl;
	for (const auto &availableLayer : availableLayers)
		std::cout << "\t" << availableLayer.layerName << std::endl;
	std::cout << std::endl;
	
	if (layersNotFound.size() > 0)
	{
		std::cout << layersNotFound.size() << " validation layers requested but unavailable: " << std::endl;
		for (const char *unavailable : layersNotFound)
			std::cout << "\t" << unavailable << std::endl;
		std::cout << std::endl;

		throw std::runtime_error("Validation layer requested but not found.");
	}

	if (layerCount == 0)
		return;

	createInfo.enabledLayerCount = VALIDATION_LAYERS.size();
	createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
	
	VkDebugReportCallbackCreateInfoEXT debugCreateInfo;
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debugCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	debugCreateInfo.pfnCallback = validationCallback;
	
	if (createDebugReportCallback(this->instance, &debugCreateInfo, nullptr, this->callback.replace()) != VK_SUCCESS)
		throw std::runtime_error("Failed to set up debug callback.");
}
#endif

void VulkanApplication::createInstance()
{
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = VulkanApplication::APPLICATION_NAME;
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	unsigned int glfwExtensionCount = 0;
	const char **glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	VkInstanceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	createInfo.enabledLayerCount = 0;
	#ifdef DEBUG
	this->enableValidationLayers(createInfo);
	#endif

	VkResult result = vkCreateInstance(&createInfo, nullptr, this->instance.replace());

	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create Vulkan instance.");

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	this->extensions.resize(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, this->extensions.data());

	#ifdef DEBUG
	std::cout << extensionCount << " available extensions: " << std::endl;
	for (const auto &extension : this->extensions)
		std::cout << "\t" << extension.extensionName << std::endl;

	std::cout << std::endl;
	#endif
}

void VulkanApplication::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	this->window = glfwCreateWindow(1920, 1080, VulkanApplication::APPLICATION_NAME, nullptr, nullptr);
}

void VulkanApplication::initVulkan()
{
	this->createInstance();
}

void VulkanApplication::createSurface()
{
	if (glfwCreateWindowSurface(this->instance, this->window, nullptr, this->surface.replace()) != VK_SUCCESS)
		throw std::runtime_error("Failed to create window surface.");
}

void VulkanApplication::findPhysicalDevices()
{
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(this->instance, &deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::runtime_error("No physical devices with Vulkan support found.");

	this->devices.clear();
	std::vector<VkPhysicalDevice> vkDevices(deviceCount);
	vkEnumeratePhysicalDevices(this->instance, &deviceCount, vkDevices.data());
	
	for (const auto &vkDevice : vkDevices)
		this->devices.push_back(Device(vkDevice));

	#ifdef DEBUG
	std::cout << this->devices.size() << " available devices." << std::endl;
	#endif
}

void VulkanApplication::pickPhysicalDevice()
{
	if (this->devices.empty())
		this->findPhysicalDevices();
	
	this->currentDevice = std::unique_ptr<Device>(&this->devices[0]);
	for (auto &dev : this->devices)
		if (dev.deviceRank() > this->currentDevice->deviceRank())
			this->currentDevice = std::unique_ptr<Device>(&dev);

	this->currentDevice->createLogicalDevice(this->surface.get());
}

void VulkanApplication::createSwapChain()
{
	this->currentSwapChain = new SwapChain(*this->currentDevice, this->surface);
	this->currentSwapChain->createSwapChain(1920, 1080);
}

void VulkanApplication::createImageViews()
{
	this->currentSwapChain->createImageViews();
}

void VulkanApplication::run()
{
	this->initWindow();
	this->initVulkan();
	this->createSurface();
	this->pickPhysicalDevice();
	this->createSwapChain();

	this->mainLoop();
}

void VulkanApplication::mainLoop()
{
	while (!glfwWindowShouldClose(this->window))
	{
		glfwPollEvents();
	}
}
