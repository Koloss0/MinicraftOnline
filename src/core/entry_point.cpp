#include <iostream>
#include <config.h>

#include "application.h"
#include "log.h"

extern Application* create_application();

int main(int argc, char** argv)
{
	Log::init();

	LOG_INFO("Starting MCO v{0}.{1}", MCO_VERSION_MAJOR, MCO_VERSION_MINOR);

	auto app = create_application();

	app->run();

	delete app;
}
