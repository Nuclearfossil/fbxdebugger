#pragma once

#include "Node.h"

#include <string>
#include <vector>

#include "glm/glm.hpp"

#define GLEW_STATIC
#include <GL\glew.h>

class Model;
typedef std::shared_ptr<Model> ModelSharedPtr;

class Model : public Node
{
public:
	Model()
	{
		mValidModel = false;
		mVAO = 0;
		mPosVBO = 0;
		mNormalVBO = 0;
		mIndexVBO = 0;
		AttributeCount = 0;
		Selected = false;
		FbxNodeRef = nullptr;
	}

	~Model()
	{
		mValidModel = false;
	}

	void BuildRenderables();

	virtual void Render(bool highlighted, Program* program);

	bool Selected;

	int AttributeCount;

	std::vector<std::string> AttributeNames;
	std::vector<glm::vec4> Vertices;
	std::vector<glm::vec4> Normals;
	std::vector<glm::u32vec3> TriangleIndices;

private:
	void InternalBuildRenderables();

private:
	GLuint mVAO;
	GLuint mPosVBO;
	GLuint mNormalVBO;
	GLuint mIndexVBO;
	bool mValidModel;
};
