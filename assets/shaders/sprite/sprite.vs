#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 col;
out vec2 texcoord;

uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(aPos, 0.0, 1.0);
	texcoord = aTexCoord;
	col = aColor;
}
