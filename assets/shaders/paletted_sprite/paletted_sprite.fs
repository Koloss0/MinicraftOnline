#version 330 core

in vec3 col;
in vec2 texcoord;

out vec4 color;

uniform sampler2D image;
uniform sampler1D palette;

void main()
{
	vec4 source_col = texture(image, texcoord);
	vec4 tex = texture(palette, source_col.r);
	color = vec4(col, source_col.a)*tex;
}
