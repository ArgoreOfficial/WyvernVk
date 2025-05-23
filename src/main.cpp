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

#include <wv/engine.h>

int main()
{
	wv::VulkanEngine engine{};
	if ( engine.Setup() )
	{
		engine.Run();
		engine.Shutdown();
	}
	else
	{
		printf( "Error occurred\n" );
	}

	return 0;
}
