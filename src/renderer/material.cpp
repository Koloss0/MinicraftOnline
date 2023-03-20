// material.cpp

#include "material.h"
#include "shader.h"

Material::Material(Shader& shader)
	: m_sampler_names{}, m_textures{}, m_shader(shader), m_id(m_next_id++)
{}

GLint Material::m_next_id = 0;

void Material::set_texture(const char* sampler_name, Texture& texture)
{
	GLint tex_unit = -1;

	// check if sampler_name is already in m_sampler_names
	for (std::vector<const char*>::size_type i = 0; i < m_sampler_names.size(); i++)
	{
		if (m_sampler_names[i] == sampler_name)
		{
			tex_unit = static_cast<GLint>(i); // get the tex unit
			break;
		}
	}

	if (tex_unit == -1) // if sampler_name was not in m_sampler_names
	{
		// create a new entry
		tex_unit = static_cast<GLint>(m_sampler_names.size());
		m_sampler_names.push_back(sampler_name);
		m_textures.push_back(&texture);
	}

	m_shader.use();
	m_shader.set_int(sampler_name, tex_unit);
	glUseProgram(0);
}

Texture& Material::get_texture() const
{
	if (m_textures.empty())
		throw std::runtime_error("Error: cannot get texture from material that contains no texture");
		
	return *(m_textures[0]);
}

void Material::use() const
{
	// bind all textures to their corresponding tex units
	for(std::vector<Texture*>::size_type i = 0; i < m_textures.size(); i++)
	{
		glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
		m_textures[i]->bind();
	}
	m_shader.use();
}
