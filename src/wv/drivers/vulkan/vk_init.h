#pragma once

#include <vulkan/vulkan.hpp>

namespace wv {

namespace Vulkan {

VkCommandPoolCreateInfo     commandPoolCreateInfo    ( uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0 );
VkCommandBufferAllocateInfo commandBufferAllocateInfo( VkCommandPool pool, uint32_t count = 1 );


}

}