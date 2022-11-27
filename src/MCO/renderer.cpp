#include <MCO/renderer.h>
#include <MCO/material.h>
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
}

void Renderer::draw(Rect rect, Material& material, glm::vec3 tint)
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

	// FIRST TRIANGLE
	m_vertices.push_back(Vertex{tr,tint});	
	m_vertices.push_back(Vertex{tl,tint});
	m_vertices.push_back(Vertex{bl,tint});

	// SECOND TRIANGLE
	m_vertices.push_back(Vertex{bl,tint});	
	m_vertices.push_back(Vertex{br,tint});	
	m_vertices.push_back(Vertex{tr,tint});	
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

	m_active_material->bind();
	m_active_material->use();

	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

	glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
	
	m_vertices.clear();
}
