#include "VulkanApplication.h"

int main(int argc, char **argv)
{
	VulkanApplication app;

	try
	{
		app.run();
	}
	catch (const std::exception &ex)
	{
		std::cerr << "ERROR: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
