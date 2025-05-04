---------------------------------
--! @file xmake.lua
--! @author Argore
--! @brief Template C++ configuration
--! @version 0.1
--! @date 2025-04-04
--! 
--! @copyright Copyright (c) 2025 Argore
---------------------------------

local PROJECT_NAME = "PaleoVk"

local VK_SDK = os.getenv("VK_SDK_PATH")
local ENGINE_PACKAGES = {
    "libsdl", 
    "vulkansdk", 
    "vk-bootstrap"
}

set_languages("clatest", "cxxlatest")

add_rules "mode.debug"
add_rules "mode.release"


add_requires(ENGINE_PACKAGES)

target (PROJECT_NAME)
    set_kind "binary" 
    
    set_basename(PROJECT_NAME .. "_$(mode)_$(arch)")
    
    set_targetdir "bin"
    set_objectdir "build/obj"
    
    add_includedirs "src/"
    add_headerfiles{ "src/**.h", "src/**.hpp" }
    add_files{ "src/**.c", "src/**.cpp" }

    add_packages(ENGINE_PACKAGES)
target_end()
