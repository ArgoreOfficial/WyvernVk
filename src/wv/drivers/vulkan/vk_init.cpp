#include "vk_init.h"

VkCommandPoolCreateInfo wv::Vulkan::commandPoolCreateInfo( uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags )
{
	VkCommandPoolCreateInfo info{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	info.queueFamilyIndex = queueFamilyIndex;
	info.flags = flags;
	return info;
}

VkCommandBufferAllocateInfo wv::Vulkan::commandBufferAllocateInfo( VkCommandPool pool, uint32_t count )
{
	VkCommandBufferAllocateInfo info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	info.commandPool = pool;
	info.commandBufferCount = count;
	info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	return info;
}
