#include <iostream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "ecs/scene.h"
#include "ecs/scene_view.h"
#include "ecs/components/transform_component.h"
#include "ecs/components/material_component.h"
#include "ecs/components/sprite_component.h"
#include "renderer/renderer.h"
#include "renderer/texture.h"
#include "renderer/material.h"
#include "renderer/shader.h"
#include "glm/ext/vector_float2.hpp"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

constexpr const char* const SPRITESHEET_PATH = "assets/images/textures/sprites.png";
constexpr const char* const BASIC_VERT_SHADER_PATH = "assets/shaders/basic/basic.vs";
constexpr const char* const BASIC_FRAG_SHADER_PATH = "assets/shaders/basic/basic.fs";
constexpr const char* const SPRITE_VERT_SHADER_PATH = "assets/shaders/sprite/sprite.vs";
constexpr const char* const SPRITE_FRAG_SHADER_PATH = "assets/shaders/sprite/sprite.fs";

const float TILE_SIZE = 64.0f;

const glm::vec2 SPAWN_POS(WINDOW_WIDTH*0.5f-TILE_SIZE*0.5f, WINDOW_HEIGHT*0.5f-TILE_SIZE*0.5f);

const float MOVEMENT_SPEED = 1.0f;

Scene g_scene;
EntityID g_player;

void handle_input(GLFWwindow* window);

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
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Minicraft Online", nullptr, nullptr);
	if (window == nullptr)
	{
 		std::cout << "FATAL ERROR: Failed to create window" << std::endl;
    		glfwTerminate();
    		return 1;
	}
	glfwMakeContextCurrent(window);
	
	// LOAD GLAD
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
    		std::cout << "FATAL ERROR: Failed to initialize GLAD" << std::endl;
    		glfwTerminate();
    		return 1;
	}

	try {	
		// SET THE VIEWPORT SIZE
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		Texture spritesheet;
		spritesheet.load(SPRITESHEET_PATH);
		
		Shader sprite_shader;
		sprite_shader.load(SPRITE_VERT_SHADER_PATH, SPRITE_FRAG_SHADER_PATH);
		sprite_shader.use();

		auto projection_mat = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), 0.0f, static_cast<float>(WINDOW_HEIGHT), -1.0f, 1.0f);
		sprite_shader.set_mat4("projection", projection_mat);

		Material sprite_material(sprite_shader);
		sprite_material.set_texture(0, spritesheet);
		
		// add grass sprites
		for (int tile_x = 0; tile_x < static_cast<int>(std::ceil(static_cast<float>(WINDOW_WIDTH) / TILE_SIZE)); tile_x++)
		{
			for (int tile_y = 0; tile_y < static_cast<int>(std::ceil(static_cast<float>(WINDOW_HEIGHT) / TILE_SIZE)); tile_y++)
			{
				EntityID grass_block = g_scene.new_entity();

				TransformComponent* trans = g_scene.assign_component<TransformComponent>(grass_block);
				trans->position = glm::vec2(static_cast<float>(tile_x)*TILE_SIZE, static_cast<float>(tile_y)*TILE_SIZE);

				MaterialComponent* mat = g_scene.assign_component<MaterialComponent>(grass_block);
				mat->material = &sprite_material;

				SpriteComponent* sprite = g_scene.assign_component<SpriteComponent>(grass_block);
				sprite->size = glm::vec2(TILE_SIZE);
				sprite->source_size = glm::vec2(16.f);
				sprite->source_position = glm::vec2(0.f, 128.f - 32.f);
			}
		}

		// add player sprite
		g_player = g_scene.new_entity();

		TransformComponent* player_trans = g_scene.assign_component<TransformComponent>(g_player);
		player_trans->position = SPAWN_POS;

		MaterialComponent* player_mat = g_scene.assign_component<MaterialComponent>(g_player);
		player_mat->material = &sprite_material;

		SpriteComponent* player_sprite = g_scene.assign_component<SpriteComponent>(g_player);
		player_sprite->size = glm::vec2(TILE_SIZE);
		player_sprite->source_size = glm::vec2(16.0f);
		player_sprite->source_position = glm::vec2(0.f, 128.f - 16.f);

		Renderer::init();

		// RENDER LOOP
		while(!glfwWindowShouldClose(window))
		{
			handle_input(window);

			// BG
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			Renderer::begin();

			for (EntityID ent : SceneView<TransformComponent, SpriteComponent, MaterialComponent>(g_scene))
			{
				TransformComponent* trans = g_scene.get_component<TransformComponent>(ent);
				MaterialComponent* material = g_scene.get_component<MaterialComponent>(ent);
				SpriteComponent* sprite = g_scene.get_component<SpriteComponent>(ent);

				Renderer::draw_rect(
					trans->position,
					sprite->size * trans->scale,
					sprite->source_position,
					sprite->source_size,
					*material->material
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
	player_trans->position.x += move_direction.x;
	player_trans->position.y += move_direction.y;
}
