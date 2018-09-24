#include "Shader.h"
#include <stdexcept>
#include <fstream>
#include <string>
#include <cassert>
#include <sstream>

Shader::Shader(const std::string& shaderCode, GLenum shaderType) :
	ObjectID(0),
	ReferenceCount(NULL)
{
	//create the shader object
	ObjectID = glCreateShader(shaderType);
	if (ObjectID == 0)
		throw std::runtime_error("glCreateShader failed");

	//set the source code
	const char* code = shaderCode.c_str();
	glShaderSource(ObjectID, 1, (const GLchar**)&code, NULL);

	//compile
	glCompileShader(ObjectID);

	//throw exception if compile error occurred
	GLint status;
	glGetShaderiv(ObjectID, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) 
	{
		std::string msg("Compile failure in shader:\n");

		GLint infoLogLength;
		glGetShaderiv(ObjectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* strInfoLog = new char[infoLogLength + 1];
		glGetShaderInfoLog(ObjectID, infoLogLength, NULL, strInfoLog);
		msg += strInfoLog;
		delete[] strInfoLog;

		glDeleteShader(ObjectID); ObjectID = 0;
		throw std::runtime_error(msg);
	}

	ReferenceCount = new unsigned;
	*ReferenceCount = 1;
}

Shader::Shader(const Shader& other) :
	ObjectID(other.ObjectID),
	ReferenceCount(other.ReferenceCount)
{
	Retain();
}

Shader::~Shader() 
{
	//_refCount will be NULL if constructor failed and threw an exception
	if (ReferenceCount) Release();
}

GLuint Shader::object() const 
{
	return ObjectID;
}

Shader& Shader::operator = (const Shader& other) 
{
	Release();
	ObjectID = other.ObjectID;
	ReferenceCount = other.ReferenceCount;
	Retain();
	return *this;
}

Shader Shader::shaderFromFile(const std::string& filePath, GLenum shaderType) 
{
	//open file
	std::ifstream f;
	f.open(filePath.c_str(), std::ios::in | std::ios::binary);
	if (!f.is_open())
	{
		throw std::runtime_error(std::string("Failed to open file: ") + filePath);
	}

	//read whole file into stringstream buffer
	std::stringstream buffer;
	buffer << f.rdbuf();

	//return new shader
	Shader shader(buffer.str(), shaderType);
	return shader;
}

void Shader::Retain() 
{
	assert(ReferenceCount);
	*ReferenceCount += 1;
}

void Shader::Release() 
{
	assert(ReferenceCount && *ReferenceCount > 0);
	*ReferenceCount -= 1;
	if (*ReferenceCount == 0) 
	{
		glDeleteShader(ObjectID); ObjectID = 0;
		delete ReferenceCount; ReferenceCount = NULL;
	}
}