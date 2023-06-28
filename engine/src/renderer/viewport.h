// viewport.h
#pragma once

#include <engine/renderer/shader.h>
#include <engine/renderer/texture.h>
#include <engine/renderer/image.h>
#include <glm/vec2.hpp>
#include <glad/glad.h>

#include <iostream>

class Viewport
{
	unsigned int m_width{}, m_height{};
	GLuint m_vao{}, m_vbo{}, m_fbo{};
	Shader* m_shader{};
	Texture* m_texture{};

	struct ViewportVertex
	{
		glm::vec2 position;
		glm::vec2 texcoord;
	};
	
public:
	Viewport(unsigned int width, unsigned int height)
		: m_width(width), m_height(height)
	{
		// GENERATE VAO & VBO
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		
		// BIND VAO & VBO
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		// SETUP VERTEX ATTRIBUTES
		// position
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ViewportVertex), (const void*)offsetof(ViewportVertex, position));
		glEnableVertexAttribArray(0);
		// texcoord
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ViewportVertex), (const void*)offsetof(ViewportVertex, texcoord));
		glEnableVertexAttribArray(1);

		static const Viewport::ViewportVertex quad_vertices[] = {
			// triangle 1
			{{0,1}, {0,1}},
			{{0,0}, {0,0}},
			{{1,0}, {1,0}},
			// triangle 2
			{{1,0}, {1,0}},
			{{1,1}, {1,1}},
			{{0,1}, {0,1}},
		};

		// PASS IN VERTEX DATA
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

		// UNBIND
		glBindVertexArray(0);

		// LOAD VIEWPORT SHADER
		m_shader->load("assets/shaders/viewport/viewport.vs", "assets/shaders/viewport/viewport.fs");

		// CREATE EMPTY IMAGE
		Image empty_image{m_width, m_height, 0};
		m_texture->load(empty_image, GL_RGB, GL_RGB);
	
		// GENERATE FRAMEBUFFER
		glGenFramebuffers(1, &m_fbo);

		// PASS IN TEXTURE
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture->get_id(), 0);
		GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, draw_buffers);

		// check status of framebuffer
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			throw std::runtime_error("ERROR: Failed to initialise framebuffer '");
		
		// UNBIND
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};

