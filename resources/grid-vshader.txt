#version 330 core

uniform mat4 mvpmatrix;

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 color;

out vec4 vertexColor;

void main()
{
    // does not alter the verticies at all
    gl_Position = mvpmatrix * vec4(vert, 1);
	vertexColor = vec4(color, 1.0f);
}