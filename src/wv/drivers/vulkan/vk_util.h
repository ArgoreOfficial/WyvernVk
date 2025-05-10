#pragma once

#include <vulkan/vulkan.h>

namespace wv {

namespace Vulkan {

void TransitionImage( VkCommandBuffer _cmd, VkImage _image, VkImageLayout _oldLayout, VkImageLayout _newLayout );

}

}