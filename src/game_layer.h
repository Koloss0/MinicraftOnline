#pragma once

#include <src/core/layer.h>
#include <src/events/window_event.h>
#include <src/renderer/texture.h>
#include <src/ecs/scene.h>
#include <src/core/common.h>

class GameLayer : public Layer
{
public:
	GameLayer();
	~GameLayer() = default;

	void on_attach() override;
	void on_detach() override;
	void on_update(double delta_time) override;

private:
	static IntRect get_connecting_floor_source(
			bool connected_v,
			bool connected_h,
			const std::vector<IntRect>& no_connection_sources,
			const std::vector<IntRect>& v_connection_sources,
			const std::vector<IntRect>& h_connection_sources,
			const std::vector<IntRect>& vh_connection_sources
			);

	std::shared_ptr<Texture> m_player_palette;
	std::shared_ptr<Texture> m_tile_palette;
	
	Scene m_scene;

	EntityID m_tilemap;
	EntityID m_player;
};
