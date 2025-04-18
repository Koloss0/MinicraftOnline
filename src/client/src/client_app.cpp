#include <engine/core/gui_application.hpp>

#include "client_game_layer.hpp"

const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
const char* WINDOW_TITLE = "Minicraft Online (Client)";

class ClientApp : public Engine::GUIApplication
{
public:
	ClientApp()
		: Engine::GUIApplication(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE)
	{
		push_layer( new ClientGameLayer() );
	}
};

Engine::Application* Engine::create_application()
{
	return new ClientApp();
}
