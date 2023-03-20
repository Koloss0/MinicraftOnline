#include "texture.h"
#include <iostream>
#include <glad/glad.h>

#include <STB/stb_image.h>

Texture::Texture()
	: m_id(0), m_width(0), m_height(0)
{
	glGenTextures(1, &m_id);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_id);
}

void Texture::load(const Image& image, GLint internal_format, GLuint source_format, GLint wrap_s, GLint wrap_t, GLint filter_min, GLint filter_mag)
{
	m_width = image.width;
	m_height = image.height;

	if (m_height == 1)
	{
		// 1D TEXTURE
		glBindTexture(GL_TEXTURE_1D, m_id);

		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, wrap_s);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, filter_min);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, filter_mag);
		glTexImage1D(GL_TEXTURE_1D, 0, internal_format, static_cast<GLsizei>(image.width), 0, source_format, GL_UNSIGNED_BYTE, image.data);
		
		glBindTexture(GL_TEXTURE_1D, 0);
	}
	else
	{
		// 2D TEXTURE
		glBindTexture(GL_TEXTURE_2D, m_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mag);
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, static_cast<GLsizei>(image.width), static_cast<GLsizei>(image.height), 0, source_format, GL_UNSIGNED_BYTE, image.data);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Texture::bind() const
{
	if (m_height == 1)
	{
		glBindTexture(GL_TEXTURE_1D, m_id);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, m_id);
	}
}

unsigned int Texture::get_width() const
{
	return m_width;
}

unsigned int Texture::get_height() const
{
	return m_height;
}
