#include <MCO/shader.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GLFW/glfw3.h>

Shader::Shader()
	: m_id(0)
{}

Shader::~Shader()
{
	if (m_id)
	{
		glDeleteProgram(m_id);
	}
	m_id = 0;
}

void Shader::load(const GLchar* vs_file_path, const GLchar* fs_file_path)
{
	m_vs_file_path = vs_file_path;
	m_fs_file_path = fs_file_path;

	std::string vs_source;
	std::string fs_source;

	try
	{
		// open files
		std::ifstream vs_file(vs_file_path);
		std::ifstream fs_file(fs_file_path);
		std::stringstream vs_sstream, fs_sstream;
		
		// read into temporary string streams
		vs_sstream << vs_file.rdbuf();
		fs_sstream << fs_file.rdbuf();

		// close file streams
		vs_file.close();
		fs_file.close();

		// convert streams to strings
		vs_source = vs_sstream.str();
		fs_source = fs_sstream.str();
	}
	catch(std::exception& e)
	{
		throw std::runtime_error("ERROR: Failed to load shader files: '" + (std::string)vs_file_path + "' and '" + (std::string)fs_file_path + "'"); 
	}

	const char* vs_source_c_str = vs_source.c_str();
	const char* fs_source_c_str = fs_source.c_str();

	// === CREATE SHADERS & PROGRAM ===
	GLuint vs, fs;

	GLint success;
	GLchar info_log[512];

	// VERTEX SHADER
	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source_c_str, nullptr);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vs, 512, nullptr, info_log);
		glDeleteShader(vs);
		throw std::runtime_error(
			"ERROR: Failed to compile vertex shader: '" + (std::string)vs_file_path +
			"'\n\tINFO LOG: " + (std::string)info_log +
			"\n\tSHADER SOURCE:\n" +
			vs_source);
	}

	// FRAGMENT SHADER
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source_c_str, nullptr);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fs, 512, nullptr, info_log);
		glDeleteShader(vs);
		glDeleteShader(fs);
		throw std::runtime_error(
			"ERROR: Failed to compile fragment shader: '" + (std::string)fs_file_path +
			"'\n\tINFO LOG: " + (std::string)info_log +
			"\n\tSHADER SOURCE:\n" +
			fs_source);
	}
	
	// PROGRAM
	m_id = glCreateProgram();
	glAttachShader(m_id, vs);
	glAttachShader(m_id, fs);
	glLinkProgram(m_id);
	glDeleteShader(vs);
	glDeleteShader(fs);
	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(m_id, 512, nullptr, info_log);
		throw std::runtime_error("ERROR: Failed to link shader program for shaders '" + (std::string)vs_file_path + "' and '" + (std::string)fs_file_path + "'\n\tINFO LOG: " + (std::string)info_log);
	}
}

void Shader::use()
{
	glUseProgram(m_id);
}

void Shader::set_bool(const GLchar* name, GLboolean value)
{
	glUniform1i(glGetUniformLocation(m_id, name), value);
}

void Shader::set_int(const GLchar* name, GLint value)
{
	glUniform1i(glGetUniformLocation(m_id, name), value);
}

void Shader::set_float(const GLchar* name, GLfloat value)
{
	glUniform1f(glGetUniformLocation(m_id, name), value);
}

void Shader::set_vec2f(const GLchar* name, const glm::vec2& value)
{
	glUniform2fv(glGetUniformLocation(m_id, name), 1, &value[0]);
}

void Shader::set_vec2f(const GLchar* name, GLfloat x, GLfloat y)
{
	glUniform2f(glGetUniformLocation(m_id, name), x, y);
}

void Shader::set_vec3f(const GLchar* name, const glm::vec3& value)
{
	glUniform3fv(glGetUniformLocation(m_id, name), 1, &value[0]);
}

void Shader::set_vec3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z)
{
	glUniform3f(glGetUniformLocation(m_id, name), x, y, z);
}

void Shader::set_vec4f(const GLchar* name, const glm::vec4& value)
{
	glUniform4fv(glGetUniformLocation(m_id, name), 1, &value[0]);
}

void Shader::set_vec4f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	glUniform4f(glGetUniformLocation(m_id, name), x, y, z, w);
}

void Shader::set_mat2(const GLchar* name, const glm::mat2& matrix)
{
	glUniformMatrix2fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::set_mat3(const GLchar* name, const glm::mat3& matrix)
{
	glUniformMatrix3fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::set_mat4(const GLchar* name, const glm::mat4& matrix)
{	
	glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, &matrix[0][0]);
}

