#pragma once

#define GLEW_STATIC
#include <GL\glew.h>

// #define GLFW_INCLUDE_GLU
// #include <GLFW/glfw3.h>

#include "graphics/Program.h"

#include <glm/mat4x4.hpp>

class Grid
{
public:
	Grid();
	~Grid();

	void Initialize(float gridSize, int gridCount, int bold0ffset);
	void SetMatrix(const glm::mat4 wvp) { mWVP = wvp; }

	void Draw();

private:
	GLint mInPosShaderRef;
	GLint mInColorShaderRef;

	GLint mWVPShaderRef;
	glm::mat4 mWVP;

	Program mProgram;
	GLuint mVAO;
	GLuint mPosVBO;
	GLuint mColorVBO;

	int mVertexCount;

};