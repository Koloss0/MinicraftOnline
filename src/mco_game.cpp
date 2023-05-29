#include "mco_game.h"

#include <src/core/log.h>
#include <src/math/math.h>
#include <glm/vec2.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/ext/vector_float2.hpp>
#include "ecs/scene.h"
#include "ecs/scene_view.h"
#include "ecs/components.h"
#include "io/image_loader.h"
#include "renderer/renderer.h"
#include <src/tiles.h>

const unsigned char TILE_SIZE_PX = 16;
const unsigned short WORLD_SIZE_CHKS = 25;
const unsigned char CHUNK_SIZE_TLS = 16;

const glm::vec2 SPAWN_POS(Renderer::VIEWPORT_WIDTH*0.5f, Renderer::VIEWPORT_HEIGHT*0.5f);

const float MOVEMENT_SPEED = 1.0f;

Scene g_scene;
EntityID g_player;

void handle_input(GLFWwindow* window);

void MCOGame::run()
{
	try {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		// CREATE PLAYER PALETTE
		std::shared_ptr<Texture> player_palette = std::make_shared<Texture>();
		player_palette->load(Image::create_palette({0xff010101,0xff004179,0xffa2dbff,0x0}));
		
		// LOAD TILE PALETTE
		std::shared_ptr<Texture> tile_palette = std::make_shared<Texture>();
		tile_palette->load(ImageLoader::load("assets/images/maps/palettes/tiles.png", false));

		// CREATE TILEMAP
		EntityID tilemap = g_scene.new_entity();
		TilemapComponent* tilemap_component = g_scene.assign_component<TilemapComponent>(tilemap);
		tilemap_component->init(WORLD_SIZE_CHKS, CHUNK_SIZE_TLS);
		tilemap_component->load(0, 0, 0, 0);
		g_scene.assign_component<TilemapRendererComponent>(tilemap);
		TransformComponent* tilemap_trans = g_scene.assign_component<TransformComponent>(tilemap);
		tilemap_trans->transform = glm::mat3(1.0f);

		// CREATE PLAYER ENTITY
		g_player = g_scene.new_entity();
		TransformComponent* player_trans = g_scene.assign_component<TransformComponent>(g_player);
		player_trans->transform = glm::translate(glm::mat3(1.0f), SPAWN_POS);

		SpriteComponent* player_sprite = g_scene.assign_component<SpriteComponent>(g_player);
		player_sprite->rect.x = -TILE_SIZE_PX*0.5f;
		player_sprite->rect.y = -TILE_SIZE_PX*0.5f;
		player_sprite->rect.width = TILE_SIZE_PX;
		player_sprite->rect.height = TILE_SIZE_PX;
		player_sprite->source_rect.width = 16;
		player_sprite->source_rect.height = 16;
		player_sprite->source_rect.x = 0;
		player_sprite->source_rect.y = -16;
		player_sprite->palette_atlas = player_palette;
		player_sprite->palette_index = 0;

		g_scene.assign_component<PlayerComponent>(g_player);

		// DeltaTime variables
		double deltaTime = 0.0;
    		double lastFrame = 0.0;

		// Terrain Pattern Randomisation Seed
		Math::randomise(); // randomise the RNG

		// RENDER LOOP
		while(!should_close())
		{
			// Calculate delta time
			double currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			handle_input();

			for (EntityID player : SceneView<PlayerComponent, SpriteComponent>(g_scene))
			{
				PlayerComponent* player_comp = g_scene.get_component<PlayerComponent>(player);
				SpriteComponent* sprite = g_scene.get_component<SpriteComponent>(player);

				player_comp->animation_time += deltaTime;
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
			for (EntityID ent : SceneView<TilemapComponent, TilemapRendererComponent, TransformComponent>(g_scene))
			{
				TilemapComponent* tilemap_cmp = g_scene.get_component<TilemapComponent>(ent);
				// TilemapRendererComponent* tilemap_renderer_cmp = g_scene.get_component<TilemapRendererComponent>(ent);
				TransformComponent* transform_cmp = g_scene.get_component<TransformComponent>(ent);

				for (unsigned short chunk_x_chks = 0; chunk_x_chks < WORLD_SIZE_CHKS; chunk_x_chks++)
				{
					for (unsigned short chunk_y_chks = 0; chunk_y_chks < WORLD_SIZE_CHKS; chunk_y_chks++)
					{
						// set seed for random pattern values
						uint16_t seed = static_cast<uint16_t>((static_cast<uint8_t>(chunk_x_chks) << 1) | static_cast<uint8_t>(chunk_y_chks));
						Math::set_seed(static_cast<unsigned int>(seed));

						for (unsigned char tile_lx_tls = 0; tile_lx_tls < CHUNK_SIZE_TLS; tile_lx_tls++)
						{
							for (unsigned char tile_ly_tls = 0; tile_ly_tls < CHUNK_SIZE_TLS; tile_ly_tls++)
							{
								unsigned int tile_x_tls = chunk_x_chks * CHUNK_SIZE_TLS + tile_lx_tls;
								unsigned int tile_y_tls = chunk_y_chks * CHUNK_SIZE_TLS + tile_ly_tls;
								
								TileID tile_id = tilemap_cmp->get_floor_tile(tile_x_tls, tile_y_tls);

								TileID tile_up =    tilemap_cmp->get_floor_tile(tile_x_tls    , tile_y_tls + 1);
								TileID tile_down =  tilemap_cmp->get_floor_tile(tile_x_tls    , tile_y_tls - 1);
								TileID tile_left =  tilemap_cmp->get_floor_tile(tile_x_tls - 1, tile_y_tls    );
								TileID tile_right = tilemap_cmp->get_floor_tile(tile_x_tls + 1, tile_y_tls    );

								bool connected_up = tile_up == tile_id;
								bool connected_down = tile_down == tile_id;
								bool connected_left = tile_left == tile_id;
								bool connected_right = tile_right == tile_id;

								TileData tile_data = TILES[tile_id];

								std::vector<IntRect> tl_quad_sources;
								std::vector<IntRect> tr_quad_sources;
								std::vector<IntRect> bl_quad_sources;
								std::vector<IntRect> br_quad_sources;

								// top left quad
								if (connected_up && connected_left)
								{
									tl_quad_sources = tile_data.floor_sources.connected_all;
								}
								else if (connected_up && !connected_left)
								{
									tl_quad_sources = tile_data.floor_sources.connected_udr;
								}
								else if (!connected_up && connected_left)
								{
									tl_quad_sources = tile_data.floor_sources.connected_dlr;
								}
								else if (!connected_up && !connected_left)
								{
									tl_quad_sources = tile_data.floor_sources.connected_dr;
								}
								
								// top right quad
								if (connected_up && connected_right)
								{
									tr_quad_sources = tile_data.floor_sources.connected_all;
								}
								else if (connected_up && !connected_right)
								{
									tr_quad_sources = tile_data.floor_sources.connected_udl;
								}
								else if (!connected_up && connected_right)
								{
									tr_quad_sources = tile_data.floor_sources.connected_dlr;
								}
								else if (!connected_up && !connected_right)
								{
									tr_quad_sources = tile_data.floor_sources.connected_dl;
								}

								// bottom left quad
								if (connected_down && connected_left)
								{
									bl_quad_sources = tile_data.floor_sources.connected_all;
								}
								else if (connected_down && !connected_left)
								{
									bl_quad_sources = tile_data.floor_sources.connected_udr;
								}
								else if (!connected_down && connected_left)
								{
									bl_quad_sources = tile_data.floor_sources.connected_ulr;
								}
								else if (!connected_down && !connected_left)
								{
									bl_quad_sources = tile_data.floor_sources.connected_ur;
								}
								
								// bottom right quad
								if (connected_down && connected_right)
								{
									br_quad_sources = tile_data.floor_sources.connected_all;
								}
								else if (connected_down && !connected_right)
								{
									br_quad_sources = tile_data.floor_sources.connected_udl;
								}
								else if (!connected_down && connected_right)
								{
									br_quad_sources = tile_data.floor_sources.connected_ulr;
								}
								else if (!connected_down && !connected_right)
								{
									br_quad_sources = tile_data.floor_sources.connected_ul;
								}
								
								constexpr unsigned int QUADRANT_SIZE_PX = static_cast<unsigned int>(static_cast<double>(TILE_SIZE_PX)/2.0);
								
								glm::vec3 quad_size = transform_cmp->transform * glm::vec3(QUADRANT_SIZE_PX, QUADRANT_SIZE_PX, 0.0f);
								
								if (tl_quad_sources.size() > 0)
								{
									glm::vec3 tl_quad_pos = transform_cmp->transform * glm::vec3(
										tile_x_tls * TILE_SIZE_PX,
										tile_y_tls * TILE_SIZE_PX + QUADRANT_SIZE_PX,
										1.0f
										);
									
									IntRect tl_source = tl_quad_sources[static_cast<std::vector<IntRect>::size_type>(Math::randomi()) % tl_quad_sources.size()];
								
									// draw top left quadrant
									Renderer::draw_rect(
										static_cast<int>(tl_quad_pos.x),
										static_cast<int>(tl_quad_pos.y),
										static_cast<int>(quad_size.x),
										static_cast<int>(quad_size.y),
										static_cast<int>(tl_source.x),
										static_cast<int>(tl_source.y),
										static_cast<int>(tl_source.width),
										static_cast<int>(tl_source.height),
										tile_palette,
										tile_data.palette_index
									);
								}

								if (tr_quad_sources.size() > 0)
								{
									glm::vec3 tr_quad_pos = transform_cmp->transform * glm::vec3(
										tile_x_tls * TILE_SIZE_PX + QUADRANT_SIZE_PX,
										tile_y_tls * TILE_SIZE_PX + QUADRANT_SIZE_PX,
										1.0f
										);
									
									IntRect tr_source = tr_quad_sources[static_cast<std::vector<IntRect>::size_type>(Math::randomi()) % tr_quad_sources.size()];
								
									// draw top right quadrant
									Renderer::draw_rect(
										static_cast<int>(tr_quad_pos.x),
										static_cast<int>(tr_quad_pos.y),
										static_cast<int>(quad_size.x),
										static_cast<int>(quad_size.y),
										static_cast<int>(tr_source.x),
										static_cast<int>(tr_source.y),
										static_cast<int>(tr_source.width),
										static_cast<int>(tr_source.height),
										tile_palette,
										tile_data.palette_index
									);
								}
								
								if (bl_quad_sources.size() > 0)
								{
									glm::vec3 bl_quad_pos = transform_cmp->transform * glm::vec3(
										tile_x_tls * TILE_SIZE_PX,
										tile_y_tls * TILE_SIZE_PX,
										1.0f
										);
								
									IntRect bl_source = bl_quad_sources[static_cast<std::vector<IntRect>::size_type>(Math::randomi()) % bl_quad_sources.size()];
									
									// draw bottom left quadrant
									Renderer::draw_rect(
										static_cast<int>(bl_quad_pos.x),
										static_cast<int>(bl_quad_pos.y),
										static_cast<int>(quad_size.x),
										static_cast<int>(quad_size.y),
										static_cast<int>(bl_source.x),
										static_cast<int>(bl_source.y),
										static_cast<int>(bl_source.width),
										static_cast<int>(bl_source.height),
										tile_palette,
										tile_data.palette_index
									);
								}
								
								if (br_quad_sources.size() > 0)
								{
									glm::vec3 br_quad_pos = transform_cmp->transform * glm::vec3(
										tile_x_tls * TILE_SIZE_PX + QUADRANT_SIZE_PX,
										tile_y_tls * TILE_SIZE_PX,
										1.0f
										);

									IntRect br_source = br_quad_sources[static_cast<std::vector<IntRect>::size_type>(Math::randomi()) % br_quad_sources.size()];

									// draw bottom right quadrant
									Renderer::draw_rect(
										static_cast<int>(br_quad_pos.x),
										static_cast<int>(br_quad_pos.y),
										static_cast<int>(quad_size.x),
										static_cast<int>(quad_size.y),
										static_cast<int>(br_source.x),
										static_cast<int>(br_source.y),
										static_cast<int>(br_source.width),
										static_cast<int>(br_source.height),
										tile_palette,
										tile_data.palette_index
									);
								}
							}
						}
					}
				}
			}

			for (EntityID ent : SceneView<TransformComponent, SpriteComponent>(g_scene))
			{
				TransformComponent* trans = g_scene.get_component<TransformComponent>(ent);
				SpriteComponent* sprite = g_scene.get_component<SpriteComponent>(ent);

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

			Renderer::draw_rect(40, 50, 16, 16, 0, 0, 16, 16, player_palette);

			Renderer::end();

			m_window->swap_buffers();
			glfwPollEvents();
			
			// check for GL errors
			GLenum error = glGetError();
                        if (error)
				throw std::runtime_error("GL ERROR: " + std::to_string(error));

		}
	}
	catch(std::exception& e)
	{
		LOG_ERROR(e.what());
	}
}

MCOGame::~MCOGame()
{}

void MCOGame::handle_input()
{
	glm::vec2 move_direction(0.0f);

	if (m_window->is_key_pressed(GLFW_KEY_W))
		move_direction.y += MOVEMENT_SPEED; 
	if (m_window->is_key_pressed(GLFW_KEY_S))
		move_direction.y -= MOVEMENT_SPEED; 
	if (m_window->is_key_pressed(GLFW_KEY_D))
		move_direction.x += MOVEMENT_SPEED; 
	if (m_window->is_key_pressed(GLFW_KEY_A))
		move_direction.x -= MOVEMENT_SPEED; 

	TransformComponent* player_trans = g_scene.get_component<TransformComponent>(g_player);
	player_trans->transform[2][0] += move_direction.x;
	player_trans->transform[2][1] += move_direction.y;
}

void MCOGame::on_key_input(int key)
{}
