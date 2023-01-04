#include "material.h"
#include "shader.h"

Material::Material(Shader& shader)
	: m_tex_unit(0), m_texture(nullptr), m_shader(shader), m_id(m_next_id++)
{}

GLint Material::m_next_id = 0;

void Material::set_texture(GLint tex_unit, Texture& texture)
{
	m_tex_unit = tex_unit;
	m_texture = &texture;
	m_shader.use();
	m_shader.set_int("image", m_tex_unit);
	glUseProgram(0);
}

Texture& Material::get_texture() const
{
	return *m_texture;
}

void Material::use() const
{
	if (m_texture)
	{
		glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + m_tex_unit));
		m_texture->bind();
	}
	m_shader.use();
}
