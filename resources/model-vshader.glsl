#version 330 core

layout (location = 0) in vec4 vert;
layout (location = 1) in vec4 normal;

out vec4 vertexColor;

uniform mat4 mvpmatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform bool Highlighted;

void main()
{
    gl_Position = mvpmatrix * vert;
	mat4 viewModel = modelMatrix * viewMatrix;

	vec4 vertexModelspace = viewModel * vert;
	if (!Highlighted)
		vertexColor = normal * 0.5f;
	else
		vertexColor = normal * 1.5f;
}