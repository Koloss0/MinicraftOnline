#include "core/application.h"

class MCOGame : public Application
{
	~MCOGame() override;
	void run() override;
	void on_key_input(int key) override;
	void handle_input();

	// TODO: try to get rid of this
	friend int ::main(int argc, char** argv);
};
