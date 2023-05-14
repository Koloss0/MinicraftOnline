#pragma once

#include <src/io/image_loader.h>
#include <glad/glad.h>

class Texture
{
	GLuint m_id;
	unsigned int m_width, m_height;

public:
	Texture();
	~Texture();

	void load(const Image& image, GLint internal_format = GL_RGBA, GLuint source_format = GL_RGBA, GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT, GLint filter_min = GL_NEAREST, GLint filter_mag = GL_NEAREST);
	void bind() const;
	unsigned int get_width() const;
	unsigned int get_height() const;
	
	GLuint get_id() const
	{
		return m_id;
	}
};
