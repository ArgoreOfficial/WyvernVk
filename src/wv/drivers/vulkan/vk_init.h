#pragma once

#include <vulkan/vulkan.hpp>

namespace wv {

namespace Vulkan {

VkCommandPoolCreateInfo     CommandPoolCreateInfo    ( uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0 );
VkCommandBufferAllocateInfo CommandBufferAllocateInfo( VkCommandPool pool, uint32_t count = 1 );

VkFenceCreateInfo     FenceCreateInfo    ( VkFenceCreateFlags _flags = 0 );
VkSemaphoreCreateInfo SemaphoreCreateInfo( VkSemaphoreCreateFlags _flags = 0 );

VkCommandBufferBeginInfo CmdBufferBeginInfo( VkCommandBufferUsageFlags _flags = 0 );

VkImageSubresourceRange ImageSubresourceRange( VkImageAspectFlags _aspectFlags );

VkSemaphoreSubmitInfo SemaphoreSubmitInfo( VkPipelineStageFlags2 _stageMask, VkSemaphore _semaphore );
VkCommandBufferSubmitInfo CmdBufferSubmitInfo( VkCommandBuffer _cmd );

VkSubmitInfo2 SubmitInfo( VkCommandBufferSubmitInfo* _cmd, VkSemaphoreSubmitInfo* _signalSemaphoreInfo = nullptr, VkSemaphoreSubmitInfo* _waitSemaphoreInfo = nullptr );
}

}