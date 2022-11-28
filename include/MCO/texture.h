#pragma once

#include <glad/glad.h>

class Texture
{
	GLuint m_id;
	unsigned int m_width, m_height;

public:
	Texture();
	~Texture();

	void load(const char* file_path, GLuint internal_format = GL_RGBA, GLuint source_format = GL_RGBA, GLuint wrap_s = GL_REPEAT, GLuint wrap_t = GL_REPEAT, GLuint filter_min = GL_NEAREST, GLuint filter_mag = GL_NEAREST);
	void bind();
	void use();
	unsigned int get_width();
	unsigned int get_height();
};
