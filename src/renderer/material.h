#pragma once

#include <glad/glad.h>
#include "shader.h"
#include "texture.h"

class Material
{
        static GLint m_next_id;
	GLint m_tex_unit;
	Texture* m_texture;
        Shader& m_shader;
        GLint m_id;
public:

        Material(Shader& shader);
        ~Material() = default;

	void set_texture(GLint tex_unit, Texture& texture);
	Texture& get_texture() const;
        void use() const;
};

