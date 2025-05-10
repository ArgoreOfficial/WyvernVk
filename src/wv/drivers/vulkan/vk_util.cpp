#include "vk_util.h"
#include <wv/drivers/vulkan/vk_init.h>

void wv::Vulkan::TransitionImage( VkCommandBuffer _cmd, VkImage _image, VkImageLayout _oldLayout, VkImageLayout _newLayout )
{
	VkImageMemoryBarrier2 imageBarrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2, nullptr };

	imageBarrier.srcStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	imageBarrier.dstStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

	imageBarrier.oldLayout = _oldLayout;
	imageBarrier.newLayout = _newLayout;


	VkImageAspectFlags aspectMask = ( _newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ) 
		? VK_IMAGE_ASPECT_DEPTH_BIT 
		: VK_IMAGE_ASPECT_COLOR_BIT;
	imageBarrier.subresourceRange = wv::Vulkan::ImageSubresourceRange( aspectMask );
	imageBarrier.image = _image;

	VkDependencyInfo dependencyInfo{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO, nullptr };
	dependencyInfo.imageMemoryBarrierCount = 1;
	dependencyInfo.pImageMemoryBarriers    = &imageBarrier;

	vkCmdPipelineBarrier2( _cmd, &dependencyInfo );
}
