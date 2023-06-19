#include <core/gui_application.h>
#include <src/core/gui_application.h>
#include <src/game_layer.h>

const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
const char* WINDOW_TITLE = "Minicraft Online";

class MCOClient : public GUIApplication
{
public:
	MCOClient()
		: GUIApplication(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE)
	{
		push_layer( new GameLayer() );
	}
};

class MCOServer : public Application
{
public:
	MCOServer()
	{
		push_layer( new GameLayer() );
	}
};

Application* create_application()
{
#ifdef COMPILE_SERVER
	return new MCOServer();
#else
	return new MCOClient();
#endif
}
