#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <glad/glad.h>
#include "material.h"

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
	static void draw_rect(const glm::vec2& position, const glm::vec2& size, const glm::vec2& source_position, const glm::vec2& source_size, Material& material, const glm::vec3& tint = glm::vec3(1.0f)); 
	static void draw_rect(const float x, const float y, const float width, const float height, const float source_x, const float source_y, const float source_width, const float source_height, Material& material, const glm::vec3& tint = glm::vec3(1.0f)); 
	static void begin();
	static void end();
	static void flush();
};
