#include <MCO/image.h>
#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>
#include <iostream>
#include <glad/glad.h>

Image::Image()
	:m_id(0), m_width(0), m_height(0)
{
	glGenTextures(1, &m_id);
}

Image::~Image()
{
	glDeleteTextures(1, &m_id);
}

void Image::load(const char* file_path, GLuint internal_format, GLuint source_format, GLuint wrap_s, GLuint wrap_t, GLuint filter_min, GLuint filter_mag)
{
	stbi_set_flip_vertically_on_load(true);

	int w, h, nr_channels;
	stbi_uc* image = stbi_load(file_path, &w, &h, &nr_channels, STBI_rgb_alpha);

	if (!image)
	{
		throw std::runtime_error("ERROR: Failed to load image at '" + (std::string)file_path + "'");
	}

	m_width = w;
	m_height = h;

	glBindTexture(GL_TEXTURE_2D, m_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mag);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, source_format, GL_UNSIGNED_BYTE, image);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Image::bind()
{
	glBindTexture(GL_TEXTURE_2D, m_id);
}

unsigned int Image::get_width()
{
	return m_width;
}

unsigned int Image::get_height()
{
	return m_height;
}
