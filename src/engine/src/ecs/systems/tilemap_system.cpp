#include <engine/ecs/systems/tilemap_system.hpp>

#include <engine/config.hpp>
#include <engine/core/log.hpp>
#include <engine/ecs/components.hpp>
#include <engine/ecs/scene.hpp>
#include <engine/ecs/scene_view.hpp>
#include <engine/renderer/renderer.hpp>
#include <engine/math/math.hpp>

namespace Engine
{
	static IntRect get_connecting_floor_source(
			bool connected_v,
			bool connected_h,
			const std::vector<IntRect>& no_connection_sources,
			const std::vector<IntRect>& v_connection_sources,
			const std::vector<IntRect>& h_connection_sources,
			const std::vector<IntRect>& vh_connection_sources
			);

	static void draw_chunk(const TilemapComponent::ChunkData& chunk,
			TilemapComponent& tilemap,
			TilemapRendererComponent& tilemap_renderer,
			PositionComponent& pos);

	TilemapSystem::TilemapSystem(const std::shared_ptr<Scene>& scene)
		: System(scene)
	{}

	void TilemapSystem::on_update(double delta)
	{
		if (auto scene = m_scene.lock())
		{
			// Draw tilemap
			for (Entity entity : scene->get_view<TilemapComponent, TilemapRendererComponent, PositionComponent>())
			{
				auto* tilemap_cmp = entity.get<TilemapComponent>();
				auto* tilemap_renderer_cmp = entity.get<TilemapRendererComponent>();
				auto* pos_cmp = entity.get<PositionComponent>();

				for (auto it = tilemap_cmp->loaded_chunks.begin(); it != tilemap_cmp->loaded_chunks.end(); it++)
				{
					const TilemapComponent::ChunkData& chunk = tilemap_cmp->loaded_chunks[it->first];

					draw_chunk(chunk, *tilemap_cmp, *tilemap_renderer_cmp, *pos_cmp);
				}
			}
		}
	}

	static void draw_chunk(const TilemapComponent::ChunkData& chunk,
			TilemapComponent& tilemap,
			TilemapRendererComponent& tilemap_renderer,
			PositionComponent& pos)
	{
		// set seed for random pattern values
		unsigned int seed = static_cast<uint16_t>((static_cast<uint8_t>(chunk.x)) << 1 | static_cast<uint8_t>(chunk.y));
		Math::set_seed(seed);

		for (unsigned char tile_lx_tls = 0; tile_lx_tls < CHUNK_SIZE_TLS; tile_lx_tls++)
		{
			for (unsigned char tile_ly_tls = 0; tile_ly_tls < CHUNK_SIZE_TLS; tile_ly_tls++)
			{
				// get the tile's global position
				const unsigned int tile_x_tls =
					static_cast<unsigned int>(chunk.x * CHUNK_SIZE_TLS + tile_lx_tls);
				const unsigned int tile_y_tls =
					static_cast<unsigned int>(chunk.y * CHUNK_SIZE_TLS + tile_ly_tls);

				TileID tile_id =
					tilemap.get_tile(tile_x_tls, tile_y_tls, 0);
				TileID tile_up =
					tilemap.get_tile(tile_x_tls, tile_y_tls + 1, 0);
				TileID tile_down =
					tilemap.get_tile(tile_x_tls    , tile_y_tls - 1, 0);
				TileID tile_left =
					tilemap.get_tile(tile_x_tls - 1, tile_y_tls    ,0);
				TileID tile_right =
					tilemap.get_tile(tile_x_tls + 1, tile_y_tls    ,0);

				const bool connected_up =    tile_up    == tile_id;
				const bool connected_down =  tile_down  == tile_id;
				const bool connected_left =  tile_left  == tile_id;
				const bool connected_right = tile_right == tile_id;

				const TileData& tile_data = TILES[tile_id];
				const TileData::FloorSourceData& floor_sources = tile_data.floor_sources;

				IntRect tl_quad_source = get_connecting_floor_source(
						connected_up,
						connected_left,
						floor_sources.connected_dr,
						floor_sources.connected_udr,
						floor_sources.connected_dlr,
						floor_sources.connected_all
				);
				
				IntRect tr_quad_source = get_connecting_floor_source(
						connected_up,
						connected_right,
						floor_sources.connected_dl,
						floor_sources.connected_udl,
						floor_sources.connected_dlr,
						floor_sources.connected_all
				);
				
				IntRect bl_quad_source = get_connecting_floor_source(
						connected_down,
						connected_left,
						floor_sources.connected_ur,
						floor_sources.connected_udr,
						floor_sources.connected_ulr,
						floor_sources.connected_all
				);
				
				IntRect br_quad_source = get_connecting_floor_source(
						connected_down,
						connected_right,
						floor_sources.connected_ul,
						floor_sources.connected_udl,
						floor_sources.connected_ulr,
						floor_sources.connected_all
				);
				
				constexpr int QUADRANT_SIZE_PX = TILE_SIZE_PX/2;
				
				const int tile_x_px = pos.x + static_cast<int>(tile_x_tls * TILE_SIZE_PX);
				const int tile_y_px = pos.y + static_cast<int>(tile_y_tls * TILE_SIZE_PX);

				// draw top left quadrant
				Renderer::draw_rect(
					tile_x_px,
					tile_y_px + QUADRANT_SIZE_PX,
					QUADRANT_SIZE_PX,
					QUADRANT_SIZE_PX,
					tl_quad_source.x,
					tl_quad_source.y,
					tl_quad_source.width,
					tl_quad_source.height,
					tilemap_renderer.palette_atlas,
					tile_data.palette_index
				);
				
				// draw top right quadrant
				Renderer::draw_rect(
					tile_x_px + QUADRANT_SIZE_PX,
					tile_y_px + QUADRANT_SIZE_PX,
					QUADRANT_SIZE_PX,
					QUADRANT_SIZE_PX,
					tr_quad_source.x,
					tr_quad_source.y,
					tr_quad_source.width,
					tr_quad_source.height,
					tilemap_renderer.palette_atlas,
					tile_data.palette_index
				);
				
				// draw bottom left quadrant
				Renderer::draw_rect(
					tile_x_px,
					tile_y_px,
					QUADRANT_SIZE_PX,
					QUADRANT_SIZE_PX,
					bl_quad_source.x,
					bl_quad_source.y,
					bl_quad_source.width,
					bl_quad_source.height,
					tilemap_renderer.palette_atlas,
					tile_data.palette_index
				);
				
				// draw bottom right quadrant
				Renderer::draw_rect(
					tile_x_px + QUADRANT_SIZE_PX,
					tile_y_px,
					QUADRANT_SIZE_PX,
					QUADRANT_SIZE_PX,
					br_quad_source.x,
					br_quad_source.y,
					br_quad_source.width,
					br_quad_source.height,
					tilemap_renderer.palette_atlas,
					tile_data.palette_index
				);
			}
		}
	}

	static IntRect get_connecting_floor_source(
		bool connected_v,
		bool connected_h,
		const std::vector<IntRect>& no_connection_sources,
		const std::vector<IntRect>& v_connection_sources,
		const std::vector<IntRect>& h_connection_sources,
		const std::vector<IntRect>& vh_connection_sources
		)
	{
		const std::vector<IntRect>* sources_vector;

		if (connected_v)
		{
			if (connected_h)
			{
				sources_vector = &vh_connection_sources;
			}
			else
			{
				sources_vector = &v_connection_sources;
			}
		}
		else
		{
			if (connected_h)
			{
				sources_vector = &h_connection_sources;
			}
			else
			{
				sources_vector = &no_connection_sources;
			}
		}

		if (sources_vector->size())
		{
			return sources_vector->operator[](static_cast<std::vector<IntRect>::size_type>(
						Math::randomi()) % sources_vector->size());
		}

		return IntRect{0,0,0,0};
	}

	void TilemapSystem::generate_chunks(TilemapComponent& tilemap,
			unsigned short start_x_chks, unsigned short start_y_chks,
			unsigned short end_x_chks, unsigned short end_y_chks)
	{
		ASSERT(start_x_chks < CHUNK_SIZE_TLS, "");
		ASSERT(start_y_chks < CHUNK_SIZE_TLS, "");
		ASSERT(end_x_chks < CHUNK_SIZE_TLS, "");
		ASSERT(end_y_chks < CHUNK_SIZE_TLS, "");

		for (unsigned short x_chks = start_x_chks; x_chks <= end_x_chks; x_chks++)
		{
			for (unsigned short y_chks = start_y_chks; y_chks <= end_y_chks; y_chks++)
			{
				// GENERATE NEW CHUNK
				
				TilemapComponent::ChunkLayerData floor_layer;
				TilemapComponent::ChunkLayerData wall_layer;

				for (unsigned int i = 0; i < CHUNK_SIZE_TLS * CHUNK_SIZE_TLS; i++)
				{
					floor_layer.tiles[i] = 1;
					floor_layer.hit_points[i] = 16;

					wall_layer.tiles[i] = 0;
					wall_layer.hit_points[i] = 16;
				}
				
				std::vector<EntityID> tile_entities{};

				tilemap.set_chunk(x_chks, y_chks, {
					x_chks,
					y_chks,
					{floor_layer, wall_layer},
					tile_entities
				});
			}
		}
	}
}
