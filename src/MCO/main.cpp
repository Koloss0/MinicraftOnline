#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <MCO/renderer.h>
#include <MCO/texture.h>
#include <MCO/material.h>
#include <MCO/shader.h>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

constexpr const char* const PLAYER_TEXTURE_PATH = "assets/images/textures/player.png";

constexpr const char* const BASIC_VERT_SHADER_PATH = "assets/shaders/basic/basic.vs";
constexpr const char* const BASIC_FRAG_SHADER_PATH = "assets/shaders/basic/basic.fs";

constexpr const char* const SPRITE_VERT_SHADER_PATH = "assets/shaders/sprite/sprite.vs";
constexpr const char* const SPRITE_FRAG_SHADER_PATH = "assets/shaders/sprite/sprite.fs";

constexpr float TILE_SIZE = 64.0f;

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
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
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
		
		Renderer::init();

		Shader sprite_shader;
		sprite_shader.load(SPRITE_VERT_SHADER_PATH, SPRITE_FRAG_SHADER_PATH);
		sprite_shader.use();

		auto projection_mat = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), 0.0f, static_cast<float>(WINDOW_HEIGHT), -1.0f, 1.0f);
		sprite_shader.set_mat4("projection", projection_mat);
		
		Texture player_texture;
		player_texture.load(PLAYER_TEXTURE_PATH);

		Material player_material(sprite_shader);
		player_material.set_texture(0, player_texture);

		// RENDER LOOP
		while(!glfwWindowShouldClose(window))
		{
			// BG
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			Renderer::begin();

			Renderer::draw(
				Rect{
					WINDOW_WIDTH*0.5f-TILE_SIZE*0.5f,
					WINDOW_HEIGHT*0.5f-TILE_SIZE*0.5f,
					TILE_SIZE,
					TILE_SIZE
				},
				glm::ivec2(0),
				glm::ivec2(16),
				player_material
			);

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
