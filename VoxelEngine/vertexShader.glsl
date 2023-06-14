#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in vec4 aBoneIds;
layout (location = 6) in vec4 aWeights;

out vec2 TexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos;

uniform vec3 lightPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
//	 FragPos = vec3(view * model * vec4(aPos, 1.0));
//	 Normal = mat3(transpose(inverse(view * model))) * aNormal;
//	 LightPos = vec3(view * vec4(lightPos, 1.0));

	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	LightPos = vec3(vec4(lightPos, 1.0));

	TexCoords = aTexCoord;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}