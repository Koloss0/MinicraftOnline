#include <iostream>
#include <config.h>

#include <src/mco_game.h>
#include "log.h"

int main(int argc, char** argv)
{
	Log::init();

	LOG_INFO("Starting MCO v{0}.{1}", MCO_VERSION_MAJOR, MCO_VERSION_MINOR);

	auto app = new MCOGame();

	app->run();

	delete app;

	return 0;
}
