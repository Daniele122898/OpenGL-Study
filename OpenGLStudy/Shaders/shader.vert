#version 330									
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norms;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

out vec4 vCol;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{																				
	gl_Position = projection * view * model * vec4(pos, 1.0); // gl_Position is the output of this shader	
	float c = clamp(pos.y, 0.0f, 1.0f);
	vCol = vec4(c,c,c, 1.0f);						

	TexCoord = tex;

	// Don't want the transformations in the last column of the model since it would fuck our normals
	Normal = mat3(transpose(inverse(model))) * norms;

	// We need the world pos of that vertex and only the xyz vec 3 values.
	FragPos = (model * vec4(pos, 1.0f)).xyz;
}