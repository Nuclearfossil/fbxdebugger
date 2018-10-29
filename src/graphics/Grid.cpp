
#include "grid.h"

#include "misc/Utils.h"

Grid::Grid() {}
Grid::~Grid() {}

bool CreateVBO(std::vector<glm::vec3> vec3list, GLuint &vbo)
{
	// make a single VBO and bind it for loading.
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, vec3list.size() * sizeof(glm::vec3), &vec3list[0], GL_STATIC_DRAW);

	return true;
}

GLuint CreateVBA(std::vector<glm::vec3> vec3list, std::vector<glm::vec3> colors, GLuint &vertVBO, GLuint &colorVBO)
{
	GLuint vao;
	GLuint vbo[2];

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vec3list.size() * sizeof(glm::vec3), &vec3list[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// unbind the VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vao;
}

//                    50              50              5
void Grid::Initialize(float gridSize, int gridCount, int bold0ffset)
{
	float gridLength = gridSize * gridCount/2.0f;
	std::vector<glm::vec3> posVec;
	std::vector<glm::vec3> colorVec;

	glm::vec3 lineColor(0.5f);
	glm::vec3 boldColor(0.8f);
	glm::vec3 majorZAxis(0.2f, 1.0f, 0.2f);
	glm::vec3 majorXAxis(0.9f, 0.8f, 0.1f);

	posVec.push_back(glm::vec3(0, 0, gridLength));
	posVec.push_back(glm::vec3(0, 0, -gridLength));
	posVec.push_back(glm::vec3(gridLength, 0, 0));
	posVec.push_back(glm::vec3(-gridLength, 0, 0));
	colorVec.push_back(majorZAxis);
	colorVec.push_back(majorZAxis);
	colorVec.push_back(majorXAxis);
	colorVec.push_back(majorXAxis);

	for (int grid = 1; grid < gridCount + 2; grid++)
	{
		posVec.push_back(glm::vec3(grid * gridSize, 0, gridLength));
		posVec.push_back(glm::vec3(grid * gridSize, 0, -gridLength));
		posVec.push_back(glm::vec3(-grid * gridSize, 0, gridLength));
		posVec.push_back(glm::vec3(-grid * gridSize, 0, -gridLength));
		posVec.push_back(glm::vec3(gridLength, 0, grid * gridSize));
		posVec.push_back(glm::vec3(-gridLength, 0, grid * gridSize));
		posVec.push_back(glm::vec3(gridLength, 0, -grid * gridSize));
		posVec.push_back(glm::vec3(-gridLength, 0, -grid * gridSize));

		glm::vec3 gridColor = lineColor;
		if ((grid % bold0ffset) == 0 || grid == gridCount)
		{
			gridColor = boldColor;
		}
		colorVec.push_back(gridColor);
		colorVec.push_back(gridColor);
		colorVec.push_back(gridColor);
		colorVec.push_back(gridColor);
		colorVec.push_back(gridColor);
		colorVec.push_back(gridColor);
		colorVec.push_back(gridColor);
		colorVec.push_back(gridColor);
	}

	mVertexCount = SizeT2UInt32(posVec.size());

	CreateVBO(posVec, mPosVBO);
	CreateVBO(colorVec, mColorVBO);


	mVertexCount = SizeT2UInt32(posVec.size());

	mVAO = CreateVBA(posVec, colorVec, mPosVBO, mColorVBO);
}

void Grid::Draw()
{
	glBindVertexArray(mVAO);
	glDrawArrays(GL_LINES, 0, mVertexCount/2);
	glBindVertexArray(0);
}
