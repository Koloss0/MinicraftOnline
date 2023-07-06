#include <engine/renderer/texture.hpp>
#include <engine/renderer/image.hpp>

#include <glad/glad.h>

#include <iostream>

Texture::Texture()
	: m_id(0), m_image() 
{
	glGenTextures(1, &m_id);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_id);
}

void Texture::load(std::shared_ptr<Image> image,
		GLint internal_format, GLuint source_format,
		GLint wrap_s, GLint wrap_t,
		GLint filter_min, GLint filter_mag)
{
	m_image = image;

	glBindTexture(GL_TEXTURE_2D, m_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mag);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, static_cast<GLsizei>(image->width), static_cast<GLsizei>(image->height), 0, source_format, GL_UNSIGNED_BYTE, image->data);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_id);
}
