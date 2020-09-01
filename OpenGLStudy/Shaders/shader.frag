#version 330

in vec4 vCol;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D tex;

void main()
{
	color = texture(tex, TexCoord);
}