#include <engine/renderer/material.hpp>
#include <engine/renderer/shader.hpp>
#include <engine/renderer/texture.hpp>
#include <stdexcept>

namespace Engine
{
	Material::Material(const std::shared_ptr<Shader>& shader)
		: m_sampler_names{}, m_textures{}, m_shader(shader), m_id(m_next_id++)
	{}

	GLint Material::m_next_id = 0;

	void Material::set_texture(const char* sampler_name, const std::shared_ptr<Texture>& texture)
	{
		GLint tex_unit = get_tex_unit_of_sampler(sampler_name);

		if (tex_unit == -1) // if sampler_name was not in m_sampler_names
		{
			// create a new entry
			tex_unit = static_cast<GLint>(m_sampler_names.size());
			m_sampler_names.push_back(sampler_name);
			m_textures.push_back(texture);
		}
		else {
			m_textures[static_cast<std::vector<Texture*>::size_type>(tex_unit)] = texture;
		}

		m_shader->use();
		m_shader->set_int(sampler_name, tex_unit);
		glUseProgram(0);
	}

	const Texture& Material::get_texture(const char* sampler_name) const
	{
		if (m_textures.empty())
			throw std::runtime_error("Error: cannot get texture from material that contains no texture");

		GLint tex_unit = get_tex_unit_of_sampler(sampler_name);

		if (tex_unit == -1)
			throw std::runtime_error(std::string("Error: failed to get texture for ") + sampler_name);

		return *(m_textures[static_cast<std::vector<std::shared_ptr<Texture>>::size_type>(tex_unit)]);
	}

	const Shader& Material::get_shader() const
	{
		return *m_shader;
	}

	void Material::use() const
	{
		// bind all textures to their corresponding tex units
		for(std::vector<Texture*>::size_type i = 0; i < m_textures.size(); i++)
		{
			glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
			m_textures[i]->bind();
		}
		m_shader->use();
	}

	GLint Material::get_tex_unit_of_sampler(const char* sampler_name) const
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

		return tex_unit;
	}
}
