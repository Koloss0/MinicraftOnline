// framebuffer.h
#pragma once

#include <glad/glad.h>
#include "texture.h"

class Framebuffer
{
	GLuint m_id = 0;
public:
	Framebuffer();
	~Framebuffer();

	void assign_texture(Texture& texture) const;
	void bind() const;
	void unbind() const;
};
