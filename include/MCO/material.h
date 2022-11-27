#pragma once

#include <glad/glad.h>
#include <MCO/shader.h>

class Material
{
        static GLuint m_next_id;
        Shader& m_shader;
        GLuint m_id;
public:

        Material(Shader& shader);
        ~Material() = default;

        void use();
        void bind();
};

