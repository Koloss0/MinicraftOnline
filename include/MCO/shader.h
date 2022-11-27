#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader
{ 
  GLuint m_id;
  std::string m_vs_file_path, m_fs_file_path;
public:
  Shader();
  ~Shader();


  void load(const GLchar* vs_file_path, const GLchar* fs_file_path);
  void use();

  void set_bool(const GLchar* name, GLboolean value);
  void set_int(const GLchar* name, GLint value);
  void set_float(const GLchar* name, GLfloat value);
  void set_vec2f(const GLchar* name, const glm::vec2& value);
  void set_vec2f(const GLchar* name, GLfloat x, GLfloat y);
  void set_vec3f(const GLchar* name, const glm::vec3& value);
  void set_vec3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z);
  void set_vec4f(const GLchar* name, const glm::vec4& value);
  void set_vec4f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  void set_mat2(const GLchar* name, const glm::mat2& matrix);
  void set_mat3(const GLchar* name, const glm::mat3& matrix);
  void set_mat4(const GLchar* name, const glm::mat4& matrix);
};
