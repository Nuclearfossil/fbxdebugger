#pragma once

#include "shader.h"
#include <vector>

class Program
{
public:
	/**
	Creates a Shader program by linking a list of Shader objects
	@param shaders  The shaders to link together to make the program
	@throws std::exception if an error occurs.
	@see Shader
	*/
	Program(const std::vector<Shader>& shaders);
	Program();
	~Program();


	/**
	@result The program's object id, as returned from glCreateProgram
	*/
	GLuint object() const;


	/**
	@result The attribute index for the given name, as returned from glGetAttribLocation.
	*/
	GLint attrib(const GLchar* attribName) const;


	/**
	@result The uniform index for the given name, as returned from glGetUniformLocation.
	*/
	GLint uniform(const GLchar* uniformName) const;


private:
	GLuint _object;

	//copying disabled
	Program(const Program&);
	const Program& operator=(const Program&);
};