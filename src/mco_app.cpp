#include <src/core/application.h>
#include <src/game_layer.h>

class MCOApp : public Application
{
public:
	MCOApp()
	{
		push_layer( new GameLayer() );
	}
};

Application* create_application()
{
	return new MCOApp();
}
