#include "src/ecs/components/world_component.h"
#include <iostream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/ext/vector_float2.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

#include <config.h>

#include "log.h"
#include "ecs/scene.h"
#include "ecs/scene_view.h"
#include "ecs/components/transform_component.h"
#include "ecs/components/sprite_component.h"
#include "ecs/components/player_component.h"
#include "ecs/components/world_component.h"
#include "ecs/components/chunk_component.h"
#include "ecs/components/tile_component.h"
#include "io/image_loader.h"
#include "renderer/renderer.h"
#include "renderer/texture.h"
#include "renderer/material.h"
#include "renderer/shader.h"
#include "renderer/colour.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

constexpr const char* const SPRITESHEET_PATH = "assets/images/textures/sprites.png";
constexpr const char* const BASIC_VERT_SHADER_PATH = "assets/shaders/basic/basic.vs";
constexpr const char* const BASIC_FRAG_SHADER_PATH = "assets/shaders/basic/basic.fs";
constexpr const char* const SPRITE_VERT_SHADER_PATH = "assets/shaders/sprite/sprite.vs";
constexpr const char* const SPRITE_FRAG_SHADER_PATH = "assets/shaders/sprite/sprite.fs";
constexpr const char* const PALETTED_SPRITE_VERT_SHADER_PATH = "assets/shaders/paletted_sprite/paletted_sprite.vs";
constexpr const char* const PALETTED_SPRITE_FRAG_SHADER_PATH = "assets/shaders/paletted_sprite/paletted_sprite.fs";

const int TILE_SIZE = 16; // in game pixels

const glm::vec2 SPAWN_POS(Renderer::VIEWPORT_WIDTH*0.5f, Renderer::VIEWPORT_HEIGHT*0.5f);

const float MOVEMENT_SPEED = 1.0f;

Scene g_scene;
EntityID g_player;

void handle_input(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{
	Log::init();

	LOG_INFO("Starting MCO v{0}.{1}", MCO_VERSION_MAJOR, MCO_VERSION_MINOR);

	// INIT GLFW
	if (!glfwInit())
	{
		LOG_ERROR("Failed to initialize GLFW");
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// CREATING A WINDOW
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Minicraft Online", nullptr, nullptr);
	if (window == nullptr)
	{
 		LOG_ERROR("Failed to create window");
    		glfwTerminate();
    		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	// LOAD GLAD
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
    		LOG_ERROR("Failed to initialize GLAD");
    		glfwTerminate();
    		return 1;
	}

	try {	
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		// CREATE PLAYER PALETTE
		Texture player_palette;
		player_palette.load(*Image::create_palette({0xff010101,0xff004179,0xffa2dbff,0x0}));
		
		// LOAD TILE PALETTE
		Texture tile_palette;
		tile_palette.load(*ImageLoader::load("assets/images/maps/palettes/tiles.png", false));

		// CREATE WORLD ENTITY
		EntityID world = g_scene.new_entity();
		WorldComponent* world_component = g_scene.assign_component<WorldComponent>(world);
		
		// CREATE CHUNKS
		for (std::size_t chunk_y = 0; chunk_y < world_component->WORLD_SIZE; chunk_y++)
		{
			world_component->chunks[chunk_y] = {};
			for (std::size_t chunk_x = 0; chunk_x < world_component->WORLD_SIZE; chunk_x++)
			{
				EntityID chunk = g_scene.new_entity();
				ChunkComponent* chunk_comp = g_scene.assign_component<ChunkComponent>(chunk);

				// create tiles
				for (std::size_t tile_y = 0; tile_y < chunk_comp->CHUNK_SIZE; tile_y++)
				{
					chunk_comp->tiles[tile_y] = {};
					for (std::size_t tile_x = 0; tile_x < chunk_comp->CHUNK_SIZE; tile_x++)
					{
						EntityID tile = g_scene.new_entity();
						TileComponent* tile_comp = g_scene.assign_component<TileComponent>(tile);
						
						TransformComponent* trans = g_scene.assign_component<TransformComponent>(tile);
						auto pos = glm::vec2(chunk_x*ChunkComponent::CHUNK_SIZE + tile_x*TILE_SIZE, chunk_y*ChunkComponent::CHUNK_SIZE + tile_y*TILE_SIZE);
						trans->transform = glm::translate(glm::mat3(1.0f), pos);
						
						//MaterialComponent* mat = g_scene.assign_component<MaterialComponent>(tile);
						//mat->material = &tile_material;

						SpriteComponent* sprite = g_scene.assign_component<SpriteComponent>(tile);
						sprite->rect.x = 0.0f;
						sprite->rect.y = 0.0f;
						sprite->rect.width = TILE_SIZE;
						sprite->rect.height = TILE_SIZE;
						sprite->source_rect.width = 16;
						sprite->source_rect.height = 16;
						sprite->source_rect.x = 0;
						sprite->source_rect.y = 128-32;
						sprite->palette_atlas = &tile_palette;

						chunk_comp->tiles[tile_y][tile_x] = tile;
					}
				}

				world_component->chunks[chunk_y][chunk_x] = chunk;

			}

		}

		// add player sprite
		g_player = g_scene.new_entity();

		TransformComponent* player_trans = g_scene.assign_component<TransformComponent>(g_player);
		player_trans->transform = glm::translate(glm::mat3(1.0f), SPAWN_POS);

		//MaterialComponent* player_mat = g_scene.assign_component<MaterialComponent>(g_player);
		//player_mat->material = &player_material;

		SpriteComponent* player_sprite = g_scene.assign_component<SpriteComponent>(g_player);
		player_sprite->rect.x = -TILE_SIZE*0.5f;
		player_sprite->rect.y = -TILE_SIZE*0.5f;
		player_sprite->rect.width = TILE_SIZE;
		player_sprite->rect.height = TILE_SIZE;
		player_sprite->source_rect.width = 16;
		player_sprite->source_rect.height = 16;
		player_sprite->source_rect.x = 0;
		player_sprite->source_rect.y = -16;
		player_sprite->palette_atlas = &player_palette;
		player_sprite->palette_index = 0;

		g_scene.assign_component<PlayerComponent>(g_player);

		// DeltaTime variables
		double deltaTime = 0.0;
    		double lastFrame = 0.0;

		Renderer::init(WINDOW_WIDTH, WINDOW_HEIGHT);

		// RENDER LOOP
		while(!glfwWindowShouldClose(window))
		{
			// Calculate delta time
			double currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			handle_input(window);

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
					*(sprite->palette_atlas),
					sprite->palette_index
				);
			}

			Renderer::end();

			glfwSwapBuffers(window);
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
  	
	glfwTerminate();
	return 0;
}


void handle_input(GLFWwindow* window)
{
	glm::vec2 move_direction(0.0f);

	if (glfwGetKey(window, GLFW_KEY_W))
		move_direction.y += MOVEMENT_SPEED; 
	if (glfwGetKey(window, GLFW_KEY_S))
		move_direction.y -= MOVEMENT_SPEED; 
	if (glfwGetKey(window, GLFW_KEY_D))
		move_direction.x += MOVEMENT_SPEED; 
	if (glfwGetKey(window, GLFW_KEY_A))
		move_direction.x -= MOVEMENT_SPEED; 

	TransformComponent* player_trans = g_scene.get_component<TransformComponent>(g_player);
	player_trans->transform[2][0] += move_direction.x;
	player_trans->transform[2][1] += move_direction.y;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Renderer::update_viewport_size(width, height);
}
