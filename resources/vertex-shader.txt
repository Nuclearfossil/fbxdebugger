#version 330 core

layout (location = 0) in vec3 vert;
// uniform mat4 mvpmatrix;

void main()
{
    // does not alter the verticies at all
    // gl_Position = mvpmatrix * vec4(vert, 1);
	gl_Position = vec4(vert, 1);
}