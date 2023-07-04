#include <engine/core/application.h>
#include "server_game_layer.h"

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
