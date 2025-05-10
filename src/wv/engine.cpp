#include "engine.h"

#include <wv/drivers/vulkan/vk_init.h>
#include <wv/drivers/vulkan/vk_util.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <VkBootstrap.h>

///////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif

#define ASSERT_RET(_statement) if(!(_statement)) return false

///////////////////////////////////////////////////////////////////////////////////////

constexpr const char* ENGINE_NAME = "wyvern_1_1"; // Wyvern version 1.1
constexpr const char* APP_NAME    = "WyvernVk";

///////////////////////////////////////////////////////////////////////////////////////

bool wv::VulkanEngine::Setup()
{
	if ( !_initWindow() )
		return false;

	_initVulkan();
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::VulkanEngine::Shutdown()
{
	_shutdownVulkan();
	_shutdownWindow();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::VulkanEngine::Run()
{
	bool running = true;
	while ( running )
	{
		SDL_Event event;
		while ( SDL_PollEvent( &event ) )
		{
			if ( event.type == SDL_QUIT ) { running = false; break; }

		}

		Update();
		Draw();

	}
}

void wv::VulkanEngine::Update()
{
}

void wv::VulkanEngine::Draw()
{
	VK_CHECK( vkWaitForFences( m_device, 1, &_getCurrentFrame().renderFence, VK_TRUE, 1000000000 ) );
	VK_CHECK( vkResetFences  ( m_device, 1, &_getCurrentFrame().renderFence ) );

	uint32_t swapchainImageIndex = 0;
	VK_CHECK( vkAcquireNextImageKHR( m_device, m_swapchain, 1000000000, _getCurrentFrame().swapchainSemaphore, VK_NULL_HANDLE, &swapchainImageIndex ) );

	VkCommandBuffer cmd = _getCurrentFrame().cmdBuffer;
	
	VK_CHECK( vkResetCommandBuffer( cmd, 0 ) );
	
	VkCommandBufferBeginInfo beginInfo = Vulkan::CmdBufferBeginInfo();
	VK_CHECK( vkBeginCommandBuffer( cmd, &beginInfo ) );
	
	{
		VkImage swapchainImage = m_swapchainImages[ swapchainImageIndex ];

		wv::Vulkan::TransitionImage( cmd, swapchainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL );

		float flash = std::abs( std::sin( m_frameNumber / 120.f));
		VkClearColorValue clearColor{ { 0.0f, 0.0f, flash, 1.0f } };

		VkImageSubresourceRange clearRange = wv::Vulkan::ImageSubresourceRange( VK_IMAGE_ASPECT_COLOR_BIT );
		vkCmdClearColorImage( cmd, swapchainImage, VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1, &clearRange );

		wv::Vulkan::TransitionImage( cmd, swapchainImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );
	}
	
	VK_CHECK( vkEndCommandBuffer( cmd ) );

	{
		VkCommandBufferSubmitInfo cmdSubmitInfo = wv::Vulkan::CmdBufferSubmitInfo( cmd );

		VkSemaphoreSubmitInfo waitInfo   = wv::Vulkan::SemaphoreSubmitInfo( VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, _getCurrentFrame().swapchainSemaphore );
		VkSemaphoreSubmitInfo signalInfo = wv::Vulkan::SemaphoreSubmitInfo( VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, _getCurrentFrame().renderSemaphore );

		VkSubmitInfo2 submitInfo = wv::Vulkan::SubmitInfo( &cmdSubmitInfo, &signalInfo, &waitInfo );

		VK_CHECK( vkQueueSubmit2( m_graphicsQueue, 1, &submitInfo, _getCurrentFrame().renderFence ) );
	}

	{
		VkPresentInfoKHR presentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, nullptr };
		presentInfo.pSwapchains    = &m_swapchain;
		presentInfo.swapchainCount = 1;

		presentInfo.pWaitSemaphores    = &_getCurrentFrame().renderSemaphore;
		presentInfo.waitSemaphoreCount = 1;

		presentInfo.pImageIndices = &swapchainImageIndex;

		VK_CHECK( vkQueuePresentKHR( m_graphicsQueue, &presentInfo ) );
		m_frameNumber++;
	}

}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::VulkanEngine::_initWindow()
{
	ASSERT_RET( SDL_Init( SDL_INIT_VIDEO ) == 0 );
	
	

	m_window = SDL_CreateWindow( 
		APP_NAME, 
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, 
		m_windowExtent.width, 
		m_windowExtent.height, 
		SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN 
	);
	ASSERT_RET( m_window != nullptr );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::VulkanEngine::_shutdownWindow()
{
	SDL_DestroyWindow( m_window );
	SDL_Quit();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::VulkanEngine::_initVulkan()
{
	// Create Vulkan Instance

	vkb::InstanceBuilder builder{};

	auto instanceRet = builder.set_app_name( ENGINE_NAME )
		.request_validation_layers( true )
		.use_default_debug_messenger()
		.require_api_version( 1, 3, 0 )
		.build();

	vkb::Instance vkbInstance = instanceRet.value();

	m_instance       = vkbInstance.instance;
	m_debugMessenger = vkbInstance.debug_messenger;


	// Select Physical Device

	SDL_Vulkan_CreateSurface( m_window, m_instance, &m_surface );

	VkPhysicalDeviceVulkan13Features features13{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
	features13.dynamicRendering = true;
	features13.synchronization2 = true;

	VkPhysicalDeviceVulkan12Features features12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	features12.bufferDeviceAddress = true;
	features12.descriptorIndexing  = true;

	vkb::PhysicalDeviceSelector selector{ vkbInstance };
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version( 1, 3 )
		.set_required_features_13( features13 )
		.set_required_features_12( features12 )
		.set_surface( m_surface )
		.select()
		.value();

	vkb::DeviceBuilder deviceBuilder{ physicalDevice };
	vkb::Device vkbDevice = deviceBuilder.build().value();

	m_device = vkbDevice.device;
	m_gpu    = physicalDevice.physical_device;


	// Setup Swapchain & Frames

	_createSwapchain( m_windowExtent.width, m_windowExtent.height );

	m_graphicsQueue = vkbDevice.get_queue( vkb::QueueType::graphics ).value();
	m_graphicsQueueFamily = vkbDevice.get_queue_index( vkb::QueueType::graphics ).value();

	_initCommands();
	_createSyncStructures();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::VulkanEngine::_shutdownVulkan()
{
	vkDeviceWaitIdle( m_device );

	for ( int i = 0; i < FRAME_OVERLAP; i++ )
	{
		vkDestroyCommandPool( m_device, m_frames[ i ].cmdPool, nullptr );

		vkDestroyFence( m_device, m_frames[ i ].renderFence, nullptr );

		vkDestroySemaphore( m_device, m_frames[ i ].renderSemaphore, nullptr );
		vkDestroySemaphore( m_device, m_frames[ i ].swapchainSemaphore, nullptr );
	}
	
	_destroySwapchain();

	vkDestroySurfaceKHR( m_instance, m_surface, nullptr );
	vkDestroyDevice( m_device, nullptr );

	vkb::destroy_debug_utils_messenger( m_instance, m_debugMessenger );
	vkDestroyInstance( m_instance, nullptr );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::VulkanEngine::_createSwapchain( uint32_t _width, uint32_t _height )
{
	vkb::SwapchainBuilder swapchainBuilder{ m_gpu, m_device, m_surface };
	m_swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;
	vkb::Swapchain vkbSwapchain = swapchainBuilder
		.set_desired_format( 
			VkSurfaceFormatKHR{ 
				.format = m_swapchainImageFormat, 
				.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR 
			} 
		)
		.set_desired_present_mode( VK_PRESENT_MODE_FIFO_KHR )
		.set_desired_extent( _width, _height )
		.add_image_usage_flags( VK_IMAGE_USAGE_TRANSFER_DST_BIT )
		.build()
		.value();

	m_swapchainExtent = vkbSwapchain.extent;

	m_swapchain = vkbSwapchain.swapchain;
	m_swapchainImages = vkbSwapchain.get_images().value();
	m_swapchainImageViews = vkbSwapchain.get_image_views().value();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::VulkanEngine::_destroySwapchain()
{
	vkDestroySwapchainKHR( m_device, m_swapchain, nullptr );

	for ( size_t i = 0; i < m_swapchainImageViews.size(); i++ )
		vkDestroyImageView( m_device, m_swapchainImageViews[ i ], nullptr );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::VulkanEngine::_initCommands()
{
	VkCommandPoolCreateInfo commandPoolInfo = wv::Vulkan::CommandPoolCreateInfo( m_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );
	
	for ( size_t i = 0; i < FRAME_OVERLAP; i++ )
	{
		VK_CHECK( vkCreateCommandPool( m_device, &commandPoolInfo, nullptr, &m_frames[ i ].cmdPool ) );

		VkCommandBufferAllocateInfo cmdAllocInfo = wv::Vulkan::CommandBufferAllocateInfo( m_frames[ i ].cmdPool );
		VK_CHECK( vkAllocateCommandBuffers( m_device, &cmdAllocInfo, &m_frames[ i ].cmdBuffer ) );
	}
}

void wv::VulkanEngine::_createSyncStructures()
{
	VkFenceCreateInfo fenceInfo = wv::Vulkan::FenceCreateInfo( VK_FENCE_CREATE_SIGNALED_BIT );
	VkSemaphoreCreateInfo semaphoreInfo = wv::Vulkan::SemaphoreCreateInfo();

	for ( size_t i = 0; i < FRAME_OVERLAP; i++ )
	{
		VK_CHECK( vkCreateFence( m_device, &fenceInfo, nullptr, &m_frames[ i ].renderFence ) );

		VK_CHECK( vkCreateSemaphore( m_device, &semaphoreInfo, nullptr, &m_frames[ i ].swapchainSemaphore ) );
		VK_CHECK( vkCreateSemaphore( m_device, &semaphoreInfo, nullptr, &m_frames[ i ].renderSemaphore ) );
	}
}
