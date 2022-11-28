#include <MCO/renderer.h>
#include <MCO/material.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glad/glad.h>
#include <MCO/rect.h>

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

void Renderer::draw(Rect rect, glm::ivec2 source_position, glm::ivec2 source_dimensions, Material& material, glm::vec3 tint)
{
	if (&material != m_active_material)
	{
		Renderer::flush();
		m_active_material = &material;
	}

	//    _____
	//   |1   /|
	//   |  /  |
	//   |/___2|
	
	glm::vec2 tl(rect.x,			rect.y + rect.height);
	glm::vec2 tr(rect.x + rect.width,	rect.y + rect.height);
	glm::vec2 bl(rect.x,			rect.y);
	glm::vec2 br(rect.x + rect.width,	rect.y);

	unsigned int img_w = m_active_material->get_texture().get_width();
	unsigned int img_h = m_active_material->get_texture().get_height();

	Rect uv_rect = Rect{ // add new constructor that takes vecs and ivecs
		(float)source_position.x,
		(float)source_position.y,
		(float)source_dimensions.x,
		(float)source_dimensions.y
	};

	uv_rect.x /= img_w;
	uv_rect.y /= img_h;
	uv_rect.width /= img_w;
	uv_rect.height /= img_h;

	glm::vec2 uv_bl = glm::vec2(uv_rect.x,			uv_rect.y);
	glm::vec2 uv_br = glm::vec2(uv_rect.x + uv_rect.width, 	uv_rect.y);
	glm::vec2 uv_tl = glm::vec2(uv_rect.x, 			uv_rect.y + uv_rect.height);
	glm::vec2 uv_tr = glm::vec2(uv_rect.x + uv_rect.width, 	uv_rect.y + uv_rect.height);

	// FIRST TRIANGLE
	m_vertices.push_back(Vertex{tr,tint,uv_tr});	
	m_vertices.push_back(Vertex{tl,tint,uv_tl});
	m_vertices.push_back(Vertex{bl,tint,uv_bl});

	// SECOND TRIANGLE
	m_vertices.push_back(Vertex{bl,tint,uv_bl});	
	m_vertices.push_back(Vertex{br,tint,uv_br});	
	m_vertices.push_back(Vertex{tr,tint,uv_tr});	
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

	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

	glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
	
	m_vertices.clear();
}
