#include "vk_init.h"

VkCommandPoolCreateInfo wv::Vulkan::CommandPoolCreateInfo( uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags )
{
	VkCommandPoolCreateInfo info{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, nullptr };
	info.queueFamilyIndex = queueFamilyIndex;
	info.flags = flags;
	return info;
}

VkCommandBufferAllocateInfo wv::Vulkan::CommandBufferAllocateInfo( VkCommandPool pool, uint32_t count )
{
	VkCommandBufferAllocateInfo info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr };
	info.commandPool = pool;
	info.commandBufferCount = count;
	info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	return info;
}

VkFenceCreateInfo wv::Vulkan::FenceCreateInfo( VkFenceCreateFlags _flags )
{
	VkFenceCreateInfo info{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr };
	info.flags = _flags;
    return info;
}

VkSemaphoreCreateInfo wv::Vulkan::SemaphoreCreateInfo( VkSemaphoreCreateFlags _flags )
{
	/// TODO: timeline semaphore
	VkSemaphoreCreateInfo info{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, nullptr };
	info.flags = _flags;
	return info;
}

VkCommandBufferBeginInfo wv::Vulkan::CmdBufferBeginInfo( VkCommandBufferUsageFlags _flags )
{
	VkCommandBufferBeginInfo info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr };
	info.pInheritanceInfo = nullptr;
	info.flags = _flags;
	return info;
}

VkImageSubresourceRange wv::Vulkan::ImageSubresourceRange( VkImageAspectFlags _aspectFlags )
{
	VkImageSubresourceRange range{};
	range.aspectMask     = _aspectFlags;
	range.baseMipLevel   = 0;
	range.levelCount     = VK_REMAINING_MIP_LEVELS;
	range.baseArrayLayer = 0;
	range.layerCount     = VK_REMAINING_ARRAY_LAYERS;
	return range;
}

VkSemaphoreSubmitInfo wv::Vulkan::SemaphoreSubmitInfo( VkPipelineStageFlags2 _stageMask, VkSemaphore _semaphore )
{
	VkSemaphoreSubmitInfo info{ VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr };
	info.semaphore = _semaphore;
	info.stageMask = _stageMask;
	info.deviceIndex = 0;
	info.value = 1;
	return info;
}

VkCommandBufferSubmitInfo wv::Vulkan::CmdBufferSubmitInfo( VkCommandBuffer _cmd )
{
	VkCommandBufferSubmitInfo info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO, nullptr };
	info.commandBuffer = _cmd;
	info.deviceMask = 0;
	return info;
}

VkSubmitInfo2 wv::Vulkan::SubmitInfo( VkCommandBufferSubmitInfo* _cmd, VkSemaphoreSubmitInfo* _signalSemaphoreInfo, VkSemaphoreSubmitInfo* _waitSemaphoreInfo )
{
	VkSubmitInfo2 info = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2, nullptr };
	
	info.waitSemaphoreInfoCount = _waitSemaphoreInfo == nullptr ? 0 : 1;
	info.pWaitSemaphoreInfos    = _waitSemaphoreInfo;

	info.signalSemaphoreInfoCount = _signalSemaphoreInfo == nullptr ? 0 : 1;
	info.pSignalSemaphoreInfos    = _signalSemaphoreInfo;

	info.commandBufferInfoCount = 1;
	info.pCommandBufferInfos    = _cmd;

	return info;
}
