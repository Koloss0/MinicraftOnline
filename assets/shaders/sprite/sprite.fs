#version 330 core

in vec3 col;
in vec2 texcoord;

out vec4 color;

uniform sampler2D image;

void main()
{
	vec4 tex = texture(image, texcoord);
	color = vec4(col, 1.0f)*tex;
}
