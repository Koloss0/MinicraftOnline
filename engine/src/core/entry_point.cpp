#include <iostream>
#include "engine/config.h"

#include <core/log.h>
#include <core/application.h>

int main(int argc, char** argv)
{
	Log::init();

	LOG_INFO("Starting MCO Engine v{0}.{1}", ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR);

	try
	{
		auto app = create_application();

		app->run();

		delete app;
	}

	catch(std::exception& e)
	{
		LOG_ERROR("ERROR: {0}", e.what());
	}
}
