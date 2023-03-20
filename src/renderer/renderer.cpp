#include <iostream>
#include <vector>
#include <memory>

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "../common.h"
#include "framebuffer.h"
#include "renderer.h"
#include "shader.h"
#include "material.h"
#include "texture.h"

namespace Renderer
{
	struct Vertex
	{
		glm::vec2 position;
		glm::vec3 colour;
		glm::vec2 texcoords;
		//float tex_id;
	};

	// BATCH DRAWING
	std::vector<Vertex> g_batch_vertices = {};
	// std::vector<uint32_t> g_indices;
	Material* g_active_material = nullptr;
	GLuint g_batch_vao = 0;
	GLuint g_batch_vbo = 0;
	Framebuffer* g_framebuffer = nullptr; // for drawing to g_screen_texture
	
	// VIEWPORT
	GLuint g_viewport_quad_vao = 0;
	GLuint g_viewport_quad_vbo = 0;
	Shader* g_viewport_quad_shader = nullptr;
	Texture* g_screen_texture = nullptr;
	IntRect g_viewport_rect = {0,0,0,0}; // glViewport settings

	void init(const int window_width, const int window_height)
	{
		// setup viewport rect
		update_viewport_size(window_width, window_height);

		// create a new texture and pass in the empty image
		g_screen_texture = new Texture();
		{
			// create an empty image
			const Image empty_image{VIEWPORT_WIDTH,VIEWPORT_HEIGHT,0}; 
			g_screen_texture->load(empty_image, GL_RGB, GL_RGB);
		}

		// setup framebuffer
		g_framebuffer = new Framebuffer();
		g_framebuffer->assign_texture(*g_screen_texture);
		
		// generate viewport quad VAO/VBO
		glGenVertexArrays(1, &g_viewport_quad_vao);
		glGenBuffers(1, &g_viewport_quad_vbo);
		
		// bind vao/vbo
		glBindVertexArray(g_viewport_quad_vao);
		glBindBuffer(GL_ARRAY_BUFFER, g_viewport_quad_vbo);

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
		g_viewport_quad_shader = new Shader();
		g_viewport_quad_shader->load("assets/shaders/viewport/viewport.vs", "assets/shaders/viewport/viewport.fs");
		g_viewport_quad_shader->use();
		g_viewport_quad_shader->set_int("screen_texture", 0); // set texture unit for screen texture
		glUseProgram(0);

		// SETUP SPRITE VAO
		glGenVertexArrays(1, &g_batch_vao); // VAOs required in core OpenGL
		glGenBuffers(1, &g_batch_vbo);

		glBindVertexArray(g_batch_vao); // bind vao
		glBindBuffer(GL_ARRAY_BUFFER, g_batch_vbo); // bind vbo to vao

		// enable attributes in vbo
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);
		
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, colour));
		glEnableVertexAttribArray(1);
		
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texcoords));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0); // unbind
	}

	void draw_rect(const int x, const int y, const int width, const int height, const int source_x, const int source_y, const int source_width, const int source_height, Material& material, const glm::vec3& tint)
	{
		if (&material != g_active_material)
		{
			flush();
			g_active_material = &material;
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
		Texture& tex = g_active_material->get_texture();
		glm::vec2 img_size(tex.get_width(), tex.get_height());
	
		glm::vec2 uv_pos = glm::vec2(source_x, source_y) / img_size;
		glm::vec2 uv_size = glm::vec2(source_width, source_height) / img_size;

		glm::vec2 uv_tl(uv_pos.x,             uv_pos.y + uv_size.y);
		glm::vec2 uv_bl = uv_pos;
		glm::vec2 uv_br(uv_pos.x + uv_size.x, uv_pos.y);
		glm::vec2 uv_tr = uv_pos + uv_size;

		// TRIANGLE 1
		g_batch_vertices.push_back({tl,tint,uv_tl});
		g_batch_vertices.push_back({bl,tint,uv_bl});
		g_batch_vertices.push_back({br,tint,uv_br});

		// TRIANGLE 2
		g_batch_vertices.push_back({br,tint,uv_br});
		g_batch_vertices.push_back({tr,tint,uv_tr});
		g_batch_vertices.push_back({tl,tint,uv_tl});
	}

	void draw_rect(const glm::ivec2& position, const glm::ivec2& size, const glm::ivec2& source_position, const glm::ivec2& source_size, Material& material, const glm::vec3& tint)
	{
		draw_rect(position.x, position.y, size.x, size.y, source_position.x, source_position.y, source_size.x, source_size.y, material, tint);
	}

	void begin()
	{
		g_active_material = nullptr;
		
		// bind framebuffer
		g_framebuffer->bind();
		
		// setup viewport size
		glViewport(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT);

		// draw background colour
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black
		glClear(GL_COLOR_BUFFER_BIT);

		// bind sprite VAO	
		glBindVertexArray(g_batch_vao);
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

		glViewport(g_viewport_rect.x, g_viewport_rect.y, g_viewport_rect.width, g_viewport_rect.height);

		glBindVertexArray(g_viewport_quad_vao);

		// set active texture unit
		glActiveTexture(GL_TEXTURE0);
		g_screen_texture->bind();
		g_viewport_quad_shader->use();

		glDrawArrays(GL_TRIANGLES, 0, 6);

	}

	void flush()
	{
		if (g_batch_vertices.empty())
			return;

		if (!g_active_material)
		{
			g_batch_vertices.clear();
			return;
		}
		
		// RENDER TO FRAMEBUFFER
		// use active material
		g_active_material->use();

		// update vertex data
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(g_batch_vertices.size() * sizeof(Vertex)), g_batch_vertices.data(), GL_STATIC_DRAW);

		// draw batch
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(g_batch_vertices.size()));
		
		// clear batch
		g_batch_vertices.clear();	
	}

	void update_viewport_size(const int width, const int height)
	{
		float w = static_cast<float>(width), h = static_cast<float>(height);
		constexpr float VIEWPORT_ASPECT_RATIO = static_cast<float>(VIEWPORT_HEIGHT) / VIEWPORT_WIDTH;

		if (h / w > VIEWPORT_ASPECT_RATIO)
		{ // window is taller than viewport
			float new_height = w*VIEWPORT_ASPECT_RATIO;
			g_viewport_rect.width = width;
			g_viewport_rect.height = static_cast<int>(new_height);
			g_viewport_rect.x = 0;
			g_viewport_rect.y = static_cast<int>(h*0.5f - new_height*0.5f);
		}
		else
		{ // window is wider or equal aspect ratio
			float new_width = h/VIEWPORT_ASPECT_RATIO;
			g_viewport_rect.width = static_cast<int>(new_width);
			g_viewport_rect.height = height;
			g_viewport_rect.x = static_cast<int>(w*0.5f - new_width*0.5f);
			g_viewport_rect.y = 0;
		}

	}
}
