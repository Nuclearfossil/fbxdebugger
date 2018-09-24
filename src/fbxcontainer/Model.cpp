#include "Model.h"

#include "graphics/Program.h"

#include "misc/Utils.h"

void Model::BuildRenderables()
{
	// Build ourself
	InternalBuildRenderables();

	// Build Children
	int childCount = SizeT2Int32(Children.size());
	for (int index = 0; index < childCount; index++)
	{
		ModelSharedPtr child = std::dynamic_pointer_cast<Model>(Children[index]);
		if (child != nullptr)
		{
			child->BuildRenderables();
		}
	}
}

void Model::InternalBuildRenderables()
{
	if (Vertices.size() == 0 || TriangleIndices.size() == 0)
	{
		mValidModel = false;
		return;
	}

	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	glGenBuffers(1, &mPosVBO);
	glGenBuffers(1, &mIndexVBO);

	glBindBuffer(GL_ARRAY_BUFFER, mPosVBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(glm::vec4), &Vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	if (Normals.size() > 0)
	{
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Set Model Normal");
		glGenBuffers(1, &mNormalVBO);

		glBindBuffer(GL_ARRAY_BUFFER, mNormalVBO);
		glBufferData(GL_ARRAY_BUFFER, Normals.size() * sizeof(glm::vec4), &Normals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glPopDebugGroup();
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexVBO);
	size_t triangleIndices = TriangleIndices.size();
	size_t indexSize = sizeof(glm::u32vec3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndices * indexSize, &TriangleIndices[0], GL_STATIC_DRAW);

	// unbind the VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	mValidModel = true;
}

void Model::Render(bool highlighted, Program* program)
{
	if (mValidModel == false) return;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(mVAO);
	GLuint highlightedID = program->uniform("Highlighted");
	glUniform1i(highlightedID, highlighted);
	glDrawElements(GL_TRIANGLES, SizeT2Int32(TriangleIndices.size()) * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
