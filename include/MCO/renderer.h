#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <glad/glad.h>
#include <MCO/rect.h>
#include <MCO/material.h>

class Renderer
{
	struct Vertex
	{
		glm::vec2 position;
		glm::vec3 colour;
		glm::vec2 texcoords;
		//float tex_id;
	};

	static std::vector<Vertex> m_vertices;
	//static std::vector<uint32_t> m_indices;

	static Material* m_active_material;

	static GLuint m_vao, m_vbo;

public:
	static void init();
	static void draw_rect(const glm::vec2& position, const glm::vec2& size, const glm::ivec2& source_position, const glm::ivec2& source_size, Material& material, const glm::vec3& tint = glm::vec3(1.0f)); 
	static void begin();
	static void end();
	static void flush();
};
