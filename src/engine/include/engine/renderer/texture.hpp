#pragma once

#include <engine/io/image_loader.hpp>

#include <glad/glad.h>

#include <memory>

class Texture
{
public:
	Texture();
	~Texture();

	void load(std::shared_ptr<Image> image,
			GLint internal_format = GL_RGBA,
			GLuint source_format = GL_RGBA,
			GLint wrap_s = GL_REPEAT,
			GLint wrap_t = GL_REPEAT,
			GLint filter_min = GL_NEAREST,
			GLint filter_mag = GL_NEAREST);
	void bind() const;
	
	inline GLuint get_id() const { return m_id; }
	inline std::shared_ptr<Image> get_image() const { return m_image; }

private:
	GLuint m_id;
	std::shared_ptr<Image> m_image;
};
