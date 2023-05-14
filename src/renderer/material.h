#pragma once

#include "shader.h"
#include "texture.h"

class Material
{
        static GLint m_next_id;
	std::vector<const char*> m_sampler_names;
	std::vector<const Texture*> m_textures;
        const Shader* m_shader;
        GLint m_id;
public:

        Material(const Shader& shader);
        ~Material() = default;

	void set_texture(const char* sampler_name, const Texture& texture);
	const Texture& get_texture() const;
	const Shader& get_shader() const;
        void use() const;
};

