#include <iostream>
#include "framebuffer.h"
#include "renderer.h"
#include "shader.h"
#include "material.h"
#include "texture.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glad/glad.h>

Material* Renderer::m_active_material = nullptr;
GLuint Renderer::m_vao = 0;
GLuint Renderer::m_vbo = 0;
std::vector<Renderer::Vertex> Renderer::m_vertices;
IntRect Renderer::m_viewport_rect = {0,0,0,0};

// TEMPORARY (NEEDS REFACTORING)
// ============================
Texture* Renderer::m_fb_texture = nullptr;
Framebuffer* Renderer::m_framebuffer = nullptr;
GLuint g_quad_vao = 0;
GLuint g_quad_vertexbuffer = 0;
Shader* g_quad_shader = nullptr;
// ============================

void Renderer::init(const int window_width, const int window_height)
{
	// setup viewport rect
	update_viewport_size(window_width, window_height);

	// create a new texture and pass in the empty image
	m_fb_texture = new Texture();
	{
		// create an empty image
		Image empty_image = Image{VIEWPORT_WIDTH,VIEWPORT_HEIGHT,0}; 
		m_fb_texture->load(empty_image, GL_RGB, GL_RGB);
	}

	// SETUP FRAMEBUFFER
	m_framebuffer = new Framebuffer();
	m_framebuffer->assign_texture(*m_fb_texture);
	
	// setup viewport quad (REFACTOR)
	glGenVertexArrays(1, &g_quad_vao);
	glBindVertexArray(g_quad_vao);
	glGenBuffers(1, &g_quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, g_quad_vertexbuffer);

	// SETUP VERTEX ATTRIBUTES
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
	glBindVertexArray(0);

	// setup viewport shaders (REFACTOR)
	g_quad_shader = new Shader();
	g_quad_shader->load("assets/shaders/viewport/viewport.vs", "assets/shaders/viewport/viewport.fs");
	g_quad_shader->use();
	g_quad_shader->set_int("screen_texture", 0);
	glUseProgram(0);

	// SETUP SPRITE VAO
	glGenVertexArrays(1, &m_vao); // VAOs required in core OpenGL
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);
	// bind vbo to vao
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	// enable attributes in vbo
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, colour));
	glEnableVertexAttribArray(1);
	
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texcoords));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Renderer::draw_rect(const int x, const int y, const int width, const int height, const int source_x, const int source_y, const int source_width, const int source_height, Material& material, const glm::vec3& tint)
{
	if (&material != m_active_material)
	{
		Renderer::flush();
		m_active_material = &material;
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
	Texture& tex = m_active_material->get_texture();
	glm::vec2 img_size(tex.get_width(), tex.get_height());

	glm::vec2 uv_pos = glm::vec2(source_x, source_y) / img_size;
	glm::vec2 uv_size = glm::vec2(source_width, source_height) / img_size;

	glm::vec2 uv_tl(uv_pos.x,             uv_pos.y + uv_size.y);
	glm::vec2 uv_bl = uv_pos;
	glm::vec2 uv_br(uv_pos.x + uv_size.x, uv_pos.y);
	glm::vec2 uv_tr = uv_pos + uv_size;

	// TRIANGLE 1
	m_vertices.push_back({tl,tint,uv_tl});
	m_vertices.push_back({bl,tint,uv_bl});
	m_vertices.push_back({br,tint,uv_br});

	// TRIANGLE 2
	m_vertices.push_back({br,tint,uv_br});
	m_vertices.push_back({tr,tint,uv_tr});
	m_vertices.push_back({tl,tint,uv_tl});

	//std::cout << "(" << uv_tl.x << ", " << uv_tl.y << "), (" << uv_bl.x << ", " << uv_bl.y << "), (" << uv_br.x << ", " << uv_bl.y << "), (" << uv_tr.x << ", " << uv_tr.y << ")" << std::endl;
}

void Renderer::draw_rect(const glm::ivec2& position, const glm::ivec2& size, const glm::ivec2& source_position, const glm::ivec2& source_size, Material& material, const glm::vec3& tint)
{
	Renderer::draw_rect(position.x, position.y, size.x, size.y, source_position.x, source_position.y, source_size.x, source_size.y, material, tint);
}

void Renderer::begin()
{
	m_active_material = nullptr;
	
	// bind framebuffer
	m_framebuffer->bind();
	
	// setup viewport size
	glViewport(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT);

	// draw background colour
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black
	glClear(GL_COLOR_BUFFER_BIT);

	// bind sprite VAO	
	glBindVertexArray(m_vao);
}

void Renderer::end()
{
	Renderer::flush();
	
	// DRAW VIEWPORT QUAD
	// bind to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// draw background colour
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(m_viewport_rect.x, m_viewport_rect.y, m_viewport_rect.width, m_viewport_rect.height);

	glBindVertexArray(g_quad_vao);

	// set active texture unit
	glActiveTexture(GL_TEXTURE0);
	m_fb_texture->bind();
	g_quad_shader->use();

	glDrawArrays(GL_TRIANGLES, 0, 6);

}

void Renderer::flush()
{
	if (m_vertices.empty())
		return;

	if (!m_active_material)
	{
		m_vertices.clear();
		return;
	}
	
	// RENDER TO FRAMEBUFFER
	// use active material
	m_active_material->use();

	// update vertex data
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_vertices.size() * sizeof(Vertex)), m_vertices.data(), GL_STATIC_DRAW);

	// draw batch
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertices.size()));
	
	// clear batch
	m_vertices.clear();	
}

void Renderer::update_viewport_size(const int width, const int height)
{
	float w = static_cast<float>(width), h = static_cast<float>(height);
	constexpr float VIEWPORT_ASPECT_RATIO = static_cast<float>(VIEWPORT_HEIGHT) / VIEWPORT_WIDTH;

	if (h / w > VIEWPORT_ASPECT_RATIO)
	{ // window is taller than viewport
	  	float new_height = w*VIEWPORT_ASPECT_RATIO;
		m_viewport_rect.width = width;
		m_viewport_rect.height = static_cast<int>(new_height);
		m_viewport_rect.x = 0;
		m_viewport_rect.y = static_cast<int>(h*0.5f - new_height*0.5f);
	}
	else
	{ // window is wider or equal aspect ratio
	  	float new_width = h/VIEWPORT_ASPECT_RATIO;
	 	m_viewport_rect.width = static_cast<int>(new_width);
		m_viewport_rect.height = height;
		m_viewport_rect.x = static_cast<int>(w*0.5f - new_width*0.5f);
		m_viewport_rect.y = 0;
	}

}
