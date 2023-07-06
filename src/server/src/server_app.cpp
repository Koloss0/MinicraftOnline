#include <engine/core/application.hpp>
#include "server_game_layer.hpp"

class ServerApp : public Application
{
public:
	ServerApp()
	{
		push_layer( new ServerGameLayer() );
	}
};

Application* create_application()
{
	return new ServerApp();
}
