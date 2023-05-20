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

const unsigned char TILE_SIZE_PX = 16;
const unsigned short WORLD_SIZE_CHKS = 5;
const unsigned char CHUNK_SIZE_BLKS = 16;

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
		tilemap_component->clear();
		tilemap_component->fill(WORLD_SIZE_CHKS, CHUNK_SIZE_BLKS, 2);
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
				// TilemapComponent* tilemap_cmp = g_scene.get_component<TilemapComponent>(ent);
				// TilemapRendererComponent* tilemap_renderer_cmp = g_scene.get_component<TilemapRendererComponent>(ent);
				TransformComponent* transform_cmp = g_scene.get_component<TransformComponent>(ent);

				for (unsigned short cx = 0; cx < WORLD_SIZE_CHKS; cx++)
				{
					unsigned int chunk_x_px = static_cast<unsigned int>(cx * CHUNK_SIZE_BLKS * TILE_SIZE_PX);

					for (unsigned short cy = 0; cy < WORLD_SIZE_CHKS; cy++)
					{
						unsigned int chunk_y_px = static_cast<unsigned int>(cy * CHUNK_SIZE_BLKS * TILE_SIZE_PX);

						// set seed for random pattern values
						uint16_t seed = static_cast<uint16_t>((static_cast<uint8_t>(cx) << 1) | static_cast<uint8_t>(cy));
						Math::set_seed(static_cast<unsigned int>(seed));
						
						for (unsigned char tx = 0; tx < CHUNK_SIZE_BLKS; tx++)
						{
							unsigned int tile_lx_px = static_cast<unsigned int>(tx * TILE_SIZE_PX);

							for (unsigned char ty = 0; ty < CHUNK_SIZE_BLKS; ty++)
							{
								unsigned int tile_ly_px = static_cast<unsigned int>(ty * TILE_SIZE_PX);
								
								for (unsigned char qx = 0; qx < 2; qx++)
								{
									for (unsigned char qy = 0; qy < 2; qy++)
									{
										constexpr unsigned int HALF_TILE_PX = static_cast<unsigned int>(static_cast<float>(TILE_SIZE_PX)/2.0f);
										unsigned long x_px = chunk_x_px + tile_lx_px + qx*HALF_TILE_PX;
										unsigned long y_px = chunk_y_px + tile_ly_px + qy*HALF_TILE_PX;

										glm::vec3 pos =  transform_cmp->transform * glm::vec3(x_px, y_px, 1.0f);
										glm::vec3 size = transform_cmp->transform * glm::vec3(HALF_TILE_PX, HALF_TILE_PX, 1.0f);
										
										// randomise source rect position
										unsigned int source_x_offset_px = static_cast<unsigned int>(Math::randomi(0,2)) * HALF_TILE_PX;
										unsigned int source_y_offset_px = static_cast<unsigned int>(Math::randomi(0,2)) * HALF_TILE_PX;

										Renderer::draw_rect(
												static_cast<int>(pos.x),
												static_cast<int>(pos.y),
												static_cast<int>(size.x),
												static_cast<int>(size.y),
												static_cast<int>(source_x_offset_px),
												static_cast<int>(source_y_offset_px-32),
												8,
												8,
												tile_palette,
												0
										);
									}
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
