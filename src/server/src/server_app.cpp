#include <engine/core/application.hpp>
#include "server_game_layer.hpp"

class ServerApp : public Engine::Application
{
public:
	ServerApp()
	{
		push_layer( new ServerGameLayer() );
	}
};

Engine::Application* Engine::create_application()
{
	return new ServerApp();
}
