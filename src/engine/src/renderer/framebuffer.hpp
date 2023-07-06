#pragma once

#include <engine/renderer/texture.hpp>

#include <glad/glad.h>

class Framebuffer
{
	GLuint m_id = 0;
public:
	Framebuffer();
	~Framebuffer();

	void assign_texture(const std::shared_ptr<Texture>& texture) const;
	void bind() const;
	void unbind() const;
};
