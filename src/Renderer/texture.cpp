#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>
#include <iostream>
#include <glad/glad.h>

Texture::Texture()
	:m_id(0), m_width(0), m_height(0)
{
	glGenTextures(1, &m_id);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_id);
}

void Texture::load(const char* file_path, GLint internal_format, GLuint source_format, GLint wrap_s, GLint wrap_t, GLint filter_min, GLint filter_mag)
{
	stbi_set_flip_vertically_on_load(true);

	int w, h, nr_channels;
	stbi_uc* image = stbi_load(file_path, &w, &h, &nr_channels, STBI_rgb_alpha);

	if (!image)
	{
		throw std::runtime_error(
				"ERROR: Failed to load image at '" +
				static_cast<std::string>(file_path) +
				"'"
			);
	}

	m_width = static_cast<unsigned int>(w);
	m_height = static_cast<unsigned int>(h);

	glBindTexture(GL_TEXTURE_2D, m_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mag);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, source_format, GL_UNSIGNED_BYTE, image);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_id);
}

unsigned int Texture::get_width() const
{
	return m_width;
}

unsigned int Texture::get_height() const
{
	return m_height;
}
