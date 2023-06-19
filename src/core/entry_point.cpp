#include <iostream>
#include <config.h>

#include "log.h"
#include "application.h"

int main(int argc, char** argv)
{
	Log::init();
#ifdef COMPILE_SERVER
	LOG_INFO("SERVER COMPILED");
#endif

	LOG_INFO("Starting MCO v{0}.{1}", MCO_VERSION_MAJOR, MCO_VERSION_MINOR);

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
