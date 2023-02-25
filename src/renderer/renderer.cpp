#include "renderer.h"
#include "material.h"
#include "texture.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glad/glad.h>

Material* Renderer::m_active_material = nullptr;
GLuint Renderer::m_vao = 0;
GLuint Renderer::m_vbo = 0;
std::vector<Renderer::Vertex> Renderer::m_vertices;

void Renderer::init()
{
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, colour));
	glEnableVertexAttribArray(1);
	
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texcoords));
	glEnableVertexAttribArray(2);
}

void Renderer::draw_rect(const float x, const float y, const float width, const float height, const float source_x, const float source_y, const float source_width, const float source_height, Material& material, const glm::vec3& tint)
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
}

void Renderer::draw_rect(const glm::vec2& position, const glm::vec2& size, const glm::vec2& source_position, const glm::vec2& source_size, Material& material, const glm::vec3& tint)
{
	Renderer::draw_rect(position.x, position.y, size.x, size.y, source_position.x, source_position.y, source_size.x, source_size.y, material, tint);
}

void Renderer::begin()
{
	m_active_material = nullptr;
}

void Renderer::end()
{
	Renderer::flush();
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

	m_active_material->use();

	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_vertices.size() * sizeof(Vertex)), m_vertices.data(), GL_STATIC_DRAW);

	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertices.size()));
	
	m_vertices.clear();
}
