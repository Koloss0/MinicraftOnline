#pragma once

#include <glad/glad.h>
#include <MCO/shader.h>
#include <MCO/texture.h>

class Material
{
        static GLuint m_next_id;
	GLuint m_tex_unit;
	Texture* m_texture;
        Shader& m_shader;
        unsigned int m_id;
public:

        Material(Shader& shader);
        ~Material() = default;

	void set_texture(GLuint tex_unit, Texture& texture);
	Texture& get_texture() const;
        void use() const;
};

