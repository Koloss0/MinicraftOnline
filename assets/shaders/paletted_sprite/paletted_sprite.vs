#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in uint aPalette;

out vec3 col;
out vec2 texcoord;
flat out uint palette;

uniform mat4 projection;
uniform vec2 cameraPos;

void main()
{
	gl_Position = projection * vec4(aPos - cameraPos, 0.0, 1.0);
	texcoord = aTexCoord;
	col = aColor;
	palette = aPalette;
}
