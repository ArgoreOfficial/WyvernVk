#pragma once

#include <vulkan/vulkan.hpp>

struct SDL_Window;

namespace wv {

struct Extent2D
{
	uint32_t width = 0;
	uint32_t height = 0;
};

class VulkanEngine
{

public:
	bool init();
	void shutdown();
	void run();


private:

	bool _initWindow();
	void _shutdownWindow();

	void _initVulkan();
	void _shutdownVulkan();

	void _createSwapchain( uint32_t _width, uint32_t _height );
	void _destroySwapchain();

	SDL_Window* m_window{ nullptr };

	Extent2D m_windowExtent{ 640, 360 };

	VkInstance m_instance{ VK_NULL_HANDLE };
	VkDebugUtilsMessengerEXT m_debugMessenger{ VK_NULL_HANDLE };
	
	VkPhysicalDevice m_gpu{ VK_NULL_HANDLE };
	VkDevice m_device{ VK_NULL_HANDLE };

	VkSurfaceKHR m_surface{ VK_NULL_HANDLE };
	VkSwapchainKHR m_swapchain{ VK_NULL_HANDLE };
	VkFormat m_swapchainImageFormat{};

	std::vector<VkImage> m_swapchainImages;
	std::vector<VkImageView> m_swapchainImageViews;

	VkExtent2D m_swapchainExtent;
};

}