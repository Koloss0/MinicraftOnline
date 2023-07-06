#include <iostream>

#include <engine/core/log.hpp>
#include <engine/core/application.hpp>

int main(int argc, char** argv)
{
	Log::init();

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
