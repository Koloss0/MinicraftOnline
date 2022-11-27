#include <MCO/material.h>
#include <MCO/shader.h>

Material::Material(Shader& shader)
	: m_shader(shader), m_id(m_next_id++)
{}

GLuint Material::m_next_id = 0;

void Material::use()
{
	m_shader.use();
}

void Material::bind()
{
}

