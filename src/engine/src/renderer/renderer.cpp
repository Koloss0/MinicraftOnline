#include <engine/renderer/renderer.h>

#include <engine/core/log.h>
#include <engine/core/common.h>
#include <engine/renderer/image.h>
#include <engine/renderer/texture.h>
#include <engine/renderer/material.h>
#include "framebuffer.h"

#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>

#include <iostream>
#include <vector>
#include <memory>

namespace Renderer
{
	// TODO: add index buffer object to renderer
	struct Vertex
	{
		glm::vec2 position;
		glm::vec3 colour;
		glm::vec2 texcoords;
		unsigned int palette;
	};

	// BATCH DRAWING
	std::vector<Vertex> batch_vertices = {};
	const char* SPRITESHEET_PATH = "assets/images/textures/sprites.png"; 
	std::shared_ptr<Material> batch_material{};
	std::shared_ptr<Texture> active_palette_atlas{};
	GLuint batch_vao = 0;
	GLuint batch_vbo = 0;
	std::shared_ptr<Framebuffer> framebuffer{}; // for drawing to screen_texture

	// VIEWPORT
	GLuint viewport_quad_vao = 0;
	GLuint viewport_quad_vbo = 0;
	std::shared_ptr<Shader> viewport_quad_shader{};
	std::shared_ptr<Texture> screen_texture{};
	IntRect viewport_rect = {0,0,0,0}; // glViewport settings

	void init(unsigned int window_width, unsigned int window_height)
	{

		// setup viewport rect
		update_viewport_size(window_width, window_height);

		// create a new texture and pass in the empty image
		screen_texture = std::make_shared<Texture>();
		{
			// create an empty image
			const std::shared_ptr<Image> empty_image = std::make_shared<Image>();
			empty_image->width = VIEWPORT_WIDTH;
			empty_image->height = VIEWPORT_HEIGHT;
			empty_image->data = 0;
			screen_texture->load(empty_image, GL_RGB, GL_RGB);
		}
		
		// setup framebuffer
		framebuffer = std::make_shared<Framebuffer>();
		framebuffer->assign_texture(screen_texture);
		
		// generate viewport quad VAO/VBO
		glGenVertexArrays(1, &viewport_quad_vao);
		glGenBuffers(1, &viewport_quad_vbo);
		
		// bind vao/vbo
		glBindVertexArray(viewport_quad_vao);
		glBindBuffer(GL_ARRAY_BUFFER, viewport_quad_vbo);

		// setup quad attributes 
		struct ViewportVertex
		{
			glm::vec2 position;
			glm::vec2 texcoord;
		};
		// position
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ViewportVertex), (const void*)offsetof(ViewportVertex, position));
		glEnableVertexAttribArray(0);
		// texcoord
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ViewportVertex), (const void*)offsetof(ViewportVertex, texcoord));
		glEnableVertexAttribArray(1);

		// pass in vertex data
		static const std::vector<ViewportVertex> quad_vertices = {
			{{-1.0f, 1.0f},  {0.0f, 1.0f}},
			{{-1.0f, -1.0f}, {0.0f, 0.0f}},
			{{1.0f, -1.0f},  {1.0f, 0.0f}},
			{{1.0f, -1.0f},  {1.0f, 0.0f}},
			{{1.0f, 1.0f},   {1.0f, 1.0f}},
			{{-1.0f, 1.0f},  {0.0f, 1.0f}},
		};

		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(quad_vertices.size() * sizeof(ViewportVertex)), quad_vertices.data(), GL_STATIC_DRAW);
		glBindVertexArray(0); // unbind quad VAO

		// setup viewport shaders
		
		viewport_quad_shader = std::make_shared<Shader>();
		viewport_quad_shader->load("assets/shaders/viewport/viewport.vs", "assets/shaders/viewport/viewport.fs");
		viewport_quad_shader->use();
		viewport_quad_shader->set_int("screen_texture", 0); // set texture unit for screen texture
		
		glUseProgram(0);
		
		// SETUP SPRITE VAO
		std::shared_ptr<Shader> batch_shader = std::make_shared<Shader>();
		batch_shader->load("assets/shaders/paletted_sprite/paletted_sprite.vs", "assets/shaders/paletted_sprite/paletted_sprite.fs");
		batch_shader->use();
		glm::mat4 projection_mat = glm::ortho(0.0f, static_cast<float>(VIEWPORT_WIDTH), 0.0f, static_cast<float>(VIEWPORT_HEIGHT), -1.0f, 1.0f);
		batch_shader->set_mat4("projection", projection_mat);
		
		// SETUP BATCH MATERIAL
		std::shared_ptr<Texture> spritesheet = std::make_shared<Texture>();
		spritesheet->load(ImageLoader::load(SPRITESHEET_PATH));
		batch_material = std::make_shared<Material>(batch_shader);
		batch_material->set_texture("image", spritesheet);
		
		glGenVertexArrays(1, &batch_vao); // VAOs required in core OpenGL
		glGenBuffers(1, &batch_vbo);

		glBindVertexArray(batch_vao); // bind vao
		glBindBuffer(GL_ARRAY_BUFFER, batch_vbo); // bind vbo to vao

		// enable attributes in vbo
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);
		
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, colour));
		glEnableVertexAttribArray(1);
		
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texcoords));
		glEnableVertexAttribArray(2);

		glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (const void*)offsetof(Vertex, palette));
		glEnableVertexAttribArray(3);

		glBindVertexArray(0); // unbind

		// set draw options
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void draw_rect(const int x, const int y, const int width, const int height, const int source_x, const int source_y, const int source_width, const int source_height, const std::shared_ptr<Texture>& palette_atlas, const unsigned char palette, const glm::vec3& tint)
	{
		if (palette_atlas != active_palette_atlas)
		{
			flush();
			active_palette_atlas = palette_atlas;
		}
		

		//    _____
		//   |\   2|
		//   |  \  |
		//   |1___\|     

		// VERTICES	
		glm::vec2 tl(x,            y + height);
		glm::vec2 bl(x,               y);
		glm::vec2 br(x + width,    y);
		glm::vec2 tr(x + width, y + height);

		// UV COORDS
		const std::shared_ptr<Image> img = batch_material->get_texture("image").get_image();
		glm::vec2 img_size(img->width, img->height);
	
		glm::vec2 uv_pos = glm::vec2(source_x, source_y) / img_size;
		glm::vec2 uv_size = glm::vec2(source_width, source_height) / img_size;

		glm::vec2 uv_tl(uv_pos.x,             uv_pos.y + uv_size.y);
		glm::vec2 uv_bl = uv_pos;
		glm::vec2 uv_br(uv_pos.x + uv_size.x, uv_pos.y);
		glm::vec2 uv_tr = uv_pos + uv_size;

		// TRIANGLE 1
		batch_vertices.push_back({tl, tint, uv_tl, palette});
		batch_vertices.push_back({bl, tint, uv_bl, palette});
		batch_vertices.push_back({br, tint, uv_br, palette});

		// TRIANGLE 2
		batch_vertices.push_back({br, tint, uv_br, palette});
		batch_vertices.push_back({tr, tint, uv_tr, palette});
		batch_vertices.push_back({tl, tint, uv_tl, palette});
	}

	void draw_rect(const glm::ivec2& position, const glm::ivec2& size, const glm::ivec2& source_position, const glm::ivec2& source_size, const std::shared_ptr<Texture>& palette_atlas, const unsigned char palette, const glm::vec3& tint)
	{
		draw_rect(position.x, position.y, size.x, size.y, source_position.x, source_position.y, source_size.x, source_size.y, palette_atlas, palette, tint);
	}

	void begin()
	{
		//active_palette_atlas = nullptr;
		
		// bind framebuffer
		framebuffer->bind();
		
		// setup viewport size
		glViewport(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT);

		// draw background colour
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black
		glClear(GL_COLOR_BUFFER_BIT);

		// bind sprite VAO	
		glBindVertexArray(batch_vao);
	}

	void end()
	{
		flush();
		
		// DRAW VIEWPORT QUAD
		// bind to default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// draw background colour
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black
		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(viewport_rect.x, viewport_rect.y, viewport_rect.width, viewport_rect.height);

		glBindVertexArray(viewport_quad_vao);

		// set active texture unit
		glActiveTexture(GL_TEXTURE0);
		screen_texture->bind();
		viewport_quad_shader->use();

		glDrawArrays(GL_TRIANGLES, 0, 6);

	}

	void flush()
	{
		if (batch_vertices.empty())
			return;

		if (!active_palette_atlas)
		{
			batch_vertices.clear();
			return;
		}
		
		//LOG_TRACE("RENDERER: Flushing {0} vertices", batch_vertices.size());
		
		// RENDER TO FRAMEBUFFER
		// bind active_palette_atlas
		batch_material->set_texture("palettes", active_palette_atlas);
		batch_material->use();

		// update vertex data
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(batch_vertices.size() * sizeof(Vertex)), batch_vertices.data(), GL_STATIC_DRAW);

		// draw batch
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(batch_vertices.size()));
		
		// clear batch
		batch_vertices.clear();	
	}

	void shutdown()
	{
		batch_material.reset();
		active_palette_atlas.reset();
		framebuffer.reset();
		viewport_quad_shader.reset();
		screen_texture.reset();
	}

	void update_viewport_size(unsigned int width, unsigned int height)
	{
		float w = static_cast<float>(width), h = static_cast<float>(height);
		constexpr float VIEWPORT_ASPECT_RATIO = static_cast<float>(VIEWPORT_HEIGHT) / VIEWPORT_WIDTH;

		if (h / w > VIEWPORT_ASPECT_RATIO)
		{ // window is taller than viewport
			float new_height = w*VIEWPORT_ASPECT_RATIO;
			viewport_rect.width = static_cast<int>(width);
			viewport_rect.height = static_cast<int>(new_height);
			viewport_rect.x = 0;
			viewport_rect.y = static_cast<int>(h*0.5f - new_height*0.5f);
		}
		else
		{ // window is wider or equal aspect ratio
			float new_width = h/VIEWPORT_ASPECT_RATIO;
			viewport_rect.width = static_cast<int>(new_width);
			viewport_rect.height = static_cast<int>(height);
			viewport_rect.x = static_cast<int>(w*0.5f - new_width*0.5f);
			viewport_rect.y = 0;
		}

	}
}
