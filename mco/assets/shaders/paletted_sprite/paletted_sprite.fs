#version 330 core

in vec3 col;
in vec2 texcoord;
flat in uint palette;

out vec4 color;

uniform sampler2D image;
uniform sampler2D palettes;

void main()
{
	vec4 source_col = texture(image, texcoord);
	vec4 tex = texelFetch(palettes, ivec2(int(source_col.r*4.0f),palette),0);
	color = vec4(col, source_col.a)*tex;
}
