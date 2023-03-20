#pragma once

#include <vector>
#include <glad/glad.h>
#include "shader.h"
#include "texture.h"

class Material
{
        static GLint m_next_id;
	std::vector<const char*> m_sampler_names;
	std::vector<Texture*> m_textures;
        Shader& m_shader;
        GLint m_id;
public:

        Material(Shader& shader);
        ~Material() = default;

	void set_texture(const char* sampler_name, Texture& texture);
	Texture& get_texture() const;
        void use() const;
};

