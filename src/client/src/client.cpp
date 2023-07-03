#include <engine/core/gui_application.h>
#include "client_game_layer.h"

const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
const char* WINDOW_TITLE = "Minicraft Online (Client)";

class Client : public GUIApplication
{
public:
	Client()
		: GUIApplication(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE)
	{
		push_layer( new ClientGameLayer() );
	}
};

Application* create_application()
{
	return new Client();
}
