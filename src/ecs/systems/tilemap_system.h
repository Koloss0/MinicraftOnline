#pragma once

#include "system.h"
#include <ecs/components.h>

class TilemapSystem : public System
{
public:
	TilemapSystem(Scene& scene);
	
	virtual void on_update(double delta) override;
	void generate_chunks(TilemapComponent& tilemap,
			unsigned short start_x_chks, unsigned short start_y_chks,
			unsigned short end_x_chks, unsigned short end_y_chks);
};
