#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glad/glad.h>

#include "../common.h"
#include "material.h"
#include "texture.h"
#include "framebuffer.h"

class Renderer
{
	struct Vertex
	{
		glm::vec2 position;
		glm::vec3 colour;
		glm::vec2 texcoords;
		//float tex_id;
	};

public:
	static constexpr int VIEWPORT_WIDTH = 256;
	static constexpr int VIEWPORT_HEIGHT = 144;

private:
	static std::vector<Vertex> m_vertices;
	//static std::vector<uint32_t> m_indices;

	static Material* m_active_material;

	static GLuint m_vao, m_vbo;
	static Framebuffer* m_framebuffer;
	static Texture* m_fb_texture;
	static IntRect m_viewport_rect;
public:
	static void init(const int window_width, const int window_height);
	static void draw_rect(const glm::ivec2& position, const glm::ivec2& size, const glm::ivec2& source_position, const glm::ivec2& source_size, Material& material, const glm::vec3& tint = glm::vec3(1.0f)); 
	static void draw_rect(const int x, const int y, const int width, const int height, const int source_x, const int source_y, const int source_width, const int source_height, Material& material, const glm::vec3& tint = glm::vec3(1.0f)); 
	static void begin();
	static void end();
	static void flush();
	static void update_viewport_size(const int width, const int height);
};
