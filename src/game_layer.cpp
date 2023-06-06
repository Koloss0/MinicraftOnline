#include "game_layer.h"
#include <glm/gtx/matrix_transform_2d.hpp>
#include <src/ecs/components.h>
#include <src/ecs/scene_view.h>
#include <src/renderer/renderer.h>

static constexpr unsigned char TILE_SIZE_PX = 16;
static constexpr unsigned short WORLD_SIZE_CHKS = 25;
static constexpr unsigned char CHUNK_SIZE_TLS = 16;

static const glm::vec2 SPAWN_POS( Renderer::VIEWPORT_WIDTH*0.5, Renderer::VIEWPORT_HEIGHT*0.5);

static constexpr float MOVEMENT_SPEED = 1.0f;

GameLayer::GameLayer()
	: m_player_palette(),
	  m_tile_palette(),
	  m_scene(),
	  m_tilemap(0),
	  m_player(0)
{}

void GameLayer::on_attach()
{
	// CREATE PLAYER PALETTE
	m_player_palette = std::make_shared<Texture>();
	m_player_palette->load(Image::create_palette({0xff010101,0xff004179,0xffa2dbff,0x0}));
	
	// LOAD TILE PALETTE
	m_tile_palette = std::make_shared<Texture>();
	m_tile_palette->load(ImageLoader::load("assets/images/maps/palettes/tiles.png", false));
	
	// CREATE TILEMAP
	EntityID tilemap = m_scene.new_entity();
	
	TilemapComponent* tilemap_component = m_scene.assign_component<TilemapComponent>(tilemap);
	tilemap_component->init(WORLD_SIZE_CHKS, CHUNK_SIZE_TLS);
	tilemap_component->load(0, 0, 2, 2);
	
	m_scene.assign_component<TilemapRendererComponent>(tilemap);
	TransformComponent* tilemap_trans = m_scene.assign_component<TransformComponent>(tilemap);
	tilemap_trans->transform = glm::mat3(1.0f);

	// CREATE PLAYER ENTITY
	m_player = m_scene.new_entity();
	TransformComponent* player_trans = m_scene.assign_component<TransformComponent>(m_player);
	player_trans->transform = glm::translate(glm::mat3(1.0f), SPAWN_POS);

	SpriteComponent* player_sprite = m_scene.assign_component<SpriteComponent>(m_player);
	player_sprite->rect.x = -TILE_SIZE_PX*0.5f;
	player_sprite->rect.y = -TILE_SIZE_PX*0.5f;
	player_sprite->rect.width = TILE_SIZE_PX;
	player_sprite->rect.height = TILE_SIZE_PX;
	player_sprite->source_rect.width = 16;
	player_sprite->source_rect.height = 16;
	player_sprite->source_rect.x = 0;
	player_sprite->source_rect.y = -16;
	player_sprite->palette_atlas = m_player_palette;
	player_sprite->palette_index = 0;

	m_scene.assign_component<PlayerComponent>(m_player);
		
	// Terrain Pattern Randomisation Seed
	Math::randomise(); // randomise the RNG
}

void GameLayer::on_detach()
{
	m_player_palette.reset();
	m_tile_palette.reset();
}

void GameLayer::on_update(double delta_time)
{
	handle_input();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (EntityID player : SceneView<PlayerComponent, SpriteComponent>(m_scene))
	{
		PlayerComponent* player_comp = m_scene.get_component<PlayerComponent>(player);
		SpriteComponent* sprite = m_scene.get_component<SpriteComponent>(player);

		player_comp->animation_time += delta_time;
		if (player_comp->animation_time >= PlayerComponent::FRAME_DURATION)
		{
			player_comp->animation_time = 0.0;
			player_comp->current_frame += 1;

			static constexpr std::size_t num_frames = sizeof(PlayerComponent::WALK_FRAMES) / sizeof(Rect);
			player_comp->current_frame = player_comp->current_frame % num_frames;

			// update sprite source Rect
			sprite->source_rect = PlayerComponent::WALK_FRAMES[player_comp->current_frame];
		}
	}

	Renderer::begin();

	// Draw tilemap
	for (EntityID ent : SceneView<TilemapComponent, TilemapRendererComponent, TransformComponent>(m_scene))
	{
		TilemapComponent* tilemap_cmp = m_scene.get_component<TilemapComponent>(ent);
		// TilemapRendererComponent* tilemap_renderer_cmp = m_scene.get_component<TilemapRendererComponent>(ent);
		TransformComponent* transform_cmp = m_scene.get_component<TransformComponent>(ent);

		const glm::vec3 origin = transform_cmp->transform * glm::vec3(0.0, 0.0, 1.0);
		const int origin_x_px = static_cast<int>(origin.x);
		const int origin_y_px = static_cast<int>(origin.y);

		for (auto it = tilemap_cmp->loaded_chunks.begin(); it != tilemap_cmp->loaded_chunks.end(); it++)
		{
			const TilemapComponent::ChunkData& chunk = tilemap_cmp->loaded_chunks[it->first];

			// set seed for random pattern values
			unsigned int seed = static_cast<uint16_t>((static_cast<uint8_t>(chunk.x)) << 1 | static_cast<uint8_t>(chunk.y));
			Math::set_seed(seed);

			for (unsigned char tile_lx_tls = 0; tile_lx_tls < CHUNK_SIZE_TLS; tile_lx_tls++)
			{
				for (unsigned char tile_ly_tls = 0; tile_ly_tls < CHUNK_SIZE_TLS; tile_ly_tls++)
				{
					// get the tile's global position
					const unsigned int tile_x_tls = static_cast<unsigned int>(chunk.x * CHUNK_SIZE_TLS + tile_lx_tls);
					const unsigned int tile_y_tls = static_cast<unsigned int>(chunk.y * CHUNK_SIZE_TLS + tile_ly_tls);

					TileID tile_id =    tilemap_cmp->get_floor_tile(tile_x_tls, tile_y_tls);
					TileID tile_up =    tilemap_cmp->get_floor_tile(tile_x_tls    , tile_y_tls + 1);
					TileID tile_down =  tilemap_cmp->get_floor_tile(tile_x_tls    , tile_y_tls - 1);
					TileID tile_left =  tilemap_cmp->get_floor_tile(tile_x_tls - 1, tile_y_tls    );
					TileID tile_right = tilemap_cmp->get_floor_tile(tile_x_tls + 1, tile_y_tls    );

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
					
					constexpr int QUADRANT_SIZE_PX = static_cast<unsigned int>(static_cast<double>(TILE_SIZE_PX)/2.0);
					
					const int tile_x_px = origin_x_px + static_cast<int>(tile_x_tls * TILE_SIZE_PX);
					const int tile_y_px = origin_y_px + static_cast<int>(tile_y_tls * TILE_SIZE_PX);

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
						m_tile_palette,
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
						m_tile_palette,
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
						m_tile_palette,
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
						m_tile_palette,
						tile_data.palette_index
					);
				}
			}
		}
	}

	for (EntityID ent : SceneView<TransformComponent, SpriteComponent>(m_scene))
	{
		TransformComponent* trans = m_scene.get_component<TransformComponent>(ent);
		SpriteComponent* sprite = m_scene.get_component<SpriteComponent>(ent);

		glm::vec3 pos = trans->transform * glm::vec3(sprite->rect.x, sprite->rect.y, 1.0f);
		glm::vec3 size = trans->transform * glm::vec3(sprite->rect.width, sprite->rect.height, 0.0f);

		Renderer::draw_rect(
			static_cast<int>(pos.x),
			static_cast<int>(pos.y),
			static_cast<int>(size.x),
			static_cast<int>(size.y),
			sprite->source_rect.x,
			sprite->source_rect.y,
			sprite->source_rect.width,
			sprite->source_rect.height,
			sprite->palette_atlas,
			sprite->palette_index
		);
	}

	Renderer::end();
}

void GameLayer::handle_input()
{

}

IntRect GameLayer::get_connecting_floor_source(
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

	if (sources_vector->size() > 0)
	{
		return sources_vector->operator[](
				static_cast<std::vector<IntRect>::size_type>(Math::randomi()) % sources_vector->size()
				);
	}

	return IntRect{0,0,0,0};
}
