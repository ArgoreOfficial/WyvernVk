/**
 * @file main.cpp
 * @author Argore
 * @brief Basic xmake C++ Project Template
 * @version 0.1
 * @date 2025-04-04
 * 
 * @copyright Copyright (c) 2025 Argore
 * 
 */

#include <stdio.h>

#include <engine.h>

int main()
{
	wv::VulkanEngine engine{};
	if ( engine.init() )
	{
		engine.run();
		engine.shutdown();
	}
	else
	{
		printf( "Error occurred\n" );
	}

	return 0;
}
