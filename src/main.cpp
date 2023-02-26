#include <iostream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "ecs/scene.h"
#include "ecs/scene_view.h"
#include "ecs/components/transform_component.h"
#include "ecs/components/material_component.h"
#include "ecs/components/sprite_component.h"
#include "ecs/components/player_component.h"
#include "renderer/renderer.h"
#include "renderer/texture.h"
#include "renderer/material.h"
#include "renderer/shader.h"
#include "glm/ext/vector_float2.hpp"

constexpr float VIEWPORT_WIDTH = 800.0f;
constexpr float VIEWPORT_HEIGHT = 600.0f;

constexpr const char* const SPRITESHEET_PATH = "assets/images/textures/sprites.png";
constexpr const char* const BASIC_VERT_SHADER_PATH = "assets/shaders/basic/basic.vs";
constexpr const char* const BASIC_FRAG_SHADER_PATH = "assets/shaders/basic/basic.fs";
constexpr const char* const SPRITE_VERT_SHADER_PATH = "assets/shaders/sprite/sprite.vs";
constexpr const char* const SPRITE_FRAG_SHADER_PATH = "assets/shaders/sprite/sprite.fs";

const float TILE_SIZE = 64.0f;

const glm::vec2 SPAWN_POS(VIEWPORT_WIDTH*0.5f-TILE_SIZE*0.5f, VIEWPORT_HEIGHT*0.5f-TILE_SIZE*0.5f);

const float MOVEMENT_SPEED = 2.0f;

Scene g_scene;
EntityID g_player;

void handle_input(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{
	// INIT GLFW
	if (!glfwInit())
	{
		std::cerr << "FATAL ERROR: GLFW failed to initialize" << std::endl;
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// CREATING A WINDOW
	GLFWwindow* window = glfwCreateWindow(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, "Minicraft Online", nullptr, nullptr);
	if (window == nullptr)
	{
 		std::cout << "FATAL ERROR: Failed to create window" << std::endl;
    		glfwTerminate();
    		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	// LOAD GLAD
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
    		std::cout << "FATAL ERROR: Failed to initialize GLAD" << std::endl;
    		glfwTerminate();
    		return 1;
	}

	try {	
		// SET THE VIEWPORT SIZE
		glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		Texture spritesheet;
		spritesheet.load(SPRITESHEET_PATH);
		
		Shader sprite_shader;
		sprite_shader.load(SPRITE_VERT_SHADER_PATH, SPRITE_FRAG_SHADER_PATH);
		sprite_shader.use();

		auto projection_mat = glm::ortho(0.0f, static_cast<float>(VIEWPORT_WIDTH), 0.0f, static_cast<float>(VIEWPORT_HEIGHT), -1.0f, 1.0f);
		sprite_shader.set_mat4("projection", projection_mat);

		Material sprite_material(sprite_shader);
		sprite_material.set_texture(0, spritesheet);
		
		// add grass sprites
		for (int tile_x = 0; tile_x < static_cast<int>(std::ceil(static_cast<float>(VIEWPORT_WIDTH) / TILE_SIZE)); tile_x++)
		{
			for (int tile_y = 0; tile_y < static_cast<int>(std::ceil(static_cast<float>(VIEWPORT_HEIGHT) / TILE_SIZE)); tile_y++)
			{
				EntityID grass_block = g_scene.new_entity();

				TransformComponent* trans = g_scene.assign_component<TransformComponent>(grass_block);
				auto pos = glm::vec2(static_cast<float>(tile_x)*TILE_SIZE, static_cast<float>(tile_y)*TILE_SIZE);

				trans->transform = glm::translate(glm::mat3(1.0f), pos);

				MaterialComponent* mat = g_scene.assign_component<MaterialComponent>(grass_block);
				mat->material = &sprite_material;

				SpriteComponent* sprite = g_scene.assign_component<SpriteComponent>(grass_block);
				sprite->rect.x = 0.0f;
				sprite->rect.y = 0.0f;
				sprite->rect.width = TILE_SIZE;
				sprite->rect.height = TILE_SIZE;
				sprite->source_rect.width = 16.f;
				sprite->source_rect.height = 16.f;
				sprite->source_rect.x = 0.f;
				sprite->source_rect.y = 128.0f-32.f;
			}
		}

		// add player sprite
		g_player = g_scene.new_entity();

		TransformComponent* player_trans = g_scene.assign_component<TransformComponent>(g_player);
		player_trans->transform = glm::translate(glm::mat3(1.0f), SPAWN_POS);

		MaterialComponent* player_mat = g_scene.assign_component<MaterialComponent>(g_player);
		player_mat->material = &sprite_material;

		SpriteComponent* player_sprite = g_scene.assign_component<SpriteComponent>(g_player);
		player_sprite->rect.width = TILE_SIZE;
		player_sprite->rect.height = TILE_SIZE;
		player_sprite->source_rect.width = 16.0f;
		player_sprite->source_rect.height = 16.0f;
		player_sprite->source_rect.x = 0.f;
		player_sprite->source_rect.y = -16.f;

		g_scene.assign_component<PlayerComponent>(g_player);

		// DeltaTime variables
		double deltaTime = 0.0;
    		double lastFrame = 0.0;

		Renderer::init();

		// RENDER LOOP
		while(!glfwWindowShouldClose(window))
		{
			// Calculate delta time
			double currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			handle_input(window);

			// BG
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

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

			for (EntityID ent : SceneView<TransformComponent, SpriteComponent, MaterialComponent>(g_scene))
			{
				TransformComponent* trans = g_scene.get_component<TransformComponent>(ent);
				MaterialComponent* material = g_scene.get_component<MaterialComponent>(ent);
				SpriteComponent* sprite = g_scene.get_component<SpriteComponent>(ent);

				glm::vec3 pos = trans->transform * glm::vec3(sprite->rect.x, sprite->rect.y, 1.0f);
				glm::vec3 size = trans->transform * glm::vec3(sprite->rect.width, sprite->rect.height, 0.0f);

				Renderer::draw_rect(
					pos.x,
					pos.y,
					size.x,
					size.y,
					sprite->source_rect.x,
					sprite->source_rect.y,
					sprite->source_rect.width,
					sprite->source_rect.height,
					*material->material
				);
			}

//			Renderer::draw_rect(64.0f, 64.0f, 64.0f, 64.0f, 0.0f, 0.0f, 32.0f, 32.0f, sprite_material);


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
		std::cerr << e.what() << std::endl;
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
	player_trans->transform = glm::translate(player_trans->transform, move_direction);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	float w = static_cast<float>(width), h = static_cast<float>(height);
	constexpr float VIEWPORT_ASPECT_RATIO = VIEWPORT_HEIGHT / VIEWPORT_WIDTH;

	if (h / w > VIEWPORT_ASPECT_RATIO)
	{
		// window is taller than viewport
		float viewport_height = w*VIEWPORT_ASPECT_RATIO;
		GLint viewport_y = static_cast<GLint>(h*0.5f - viewport_height*0.5f);
		glViewport(0, viewport_y, width, static_cast<GLint>(viewport_height));
		glScissor(0, viewport_y, width, static_cast<GLint>(viewport_height));
	}
	else
	{
		// window is wider or equal aspect ratio
		float viewport_width = h/VIEWPORT_ASPECT_RATIO;
		GLint viewport_x = static_cast<GLint>(w*0.5f - viewport_width*0.5f);
		glViewport(viewport_x, 0, static_cast<GLint>(viewport_width), height);
		glScissor(viewport_x, 0, static_cast<GLint>(viewport_width), height);
	}
}
