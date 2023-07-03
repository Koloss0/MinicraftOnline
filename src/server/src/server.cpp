#include <engine/core/application.h>
#include "server_game_layer.h"

class Server : public Application
{
public:
	Server()
	{
		push_layer( new ServerGameLayer() );
	}
};

Application* create_application()
{
	return new Server();
}
