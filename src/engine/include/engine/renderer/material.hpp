#pragma once

#include "shader.hpp"
#include "texture.hpp"
#include <memory>

class Material
{
        static GLint m_next_id;
	std::vector<const char*> m_sampler_names;
	std::vector<std::shared_ptr<Texture>> m_textures;
	std::shared_ptr<const Shader> m_shader;
        GLint m_id;
public:

        Material(const std::shared_ptr<Shader>& shader);
        ~Material() = default;

	void set_texture(const char* sampler_name, const std::shared_ptr<Texture>& texture);
	const Texture& get_texture(const char* sampler_name) const;
	const Shader& get_shader() const;
        void use() const;
private:
	GLint get_tex_unit_of_sampler(const char* sampler_name) const;
};

