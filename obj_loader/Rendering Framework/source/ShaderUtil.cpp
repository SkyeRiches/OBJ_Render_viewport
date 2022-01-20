#include "../include/ShaderUtil.h"
#include "../include/Utility.h"
#include <glad/glad.h>
#include <iostream>

// Static instance of shader util
ShaderUtil* ShaderUtil::mInstance = nullptr;
// SIngleton creation, fetch and destroy functionality
ShaderUtil* ShaderUtil::GetInstance()
{
	if (mInstance == nullptr)
	{
		return ShaderUtil::CreateInstance();
	}
	return mInstance;
}

ShaderUtil* ShaderUtil::CreateInstance()
{
	if (mInstance == nullptr)
	{
		mInstance = new ShaderUtil();
	}
	else
	{
		// Print to console that attempt to create multiple instane of shaderutil
		std::cout << "Attempt to create multiple instances of ShaderUtil" << std::endl;
	}
	return mInstance;
}

void ShaderUtil::DestroyInstance()
{
	if (mInstance != nullptr)
	{
		delete mInstance;
		mInstance = nullptr;
	}
	else
	{
		// Print to console that attempt to destroy null instance of shader util
		std::cout << "Attempt to destroy null instance of ShaderUtil" << std::endl;
	}
}

ShaderUtil::ShaderUtil()
{

}

ShaderUtil::~ShaderUtil()
{
	// Delete any shaders that have not been unloaded
	for (auto iter = mShaders.begin(); iter != mShaders.end(); iter++)
	{
		glDeleteShader(*iter);
	}
	// destory any programs that are still dangling about
	for (auto iter = mPrograms.begin(); iter != mPrograms.end(); iter++)
	{
		glDeleteProgram(*iter);
	}
}

unsigned int ShaderUtil::loadShader(const char* a_filename, unsigned int a_type)
{
	ShaderUtil* instance = ShaderUtil::GetInstance();
	return instance->loadShaderInternal(a_filename, a_type);
}

unsigned int ShaderUtil::loadShaderInternal(const char* a_filename, unsigned int a_type)
{
	// INteger to test for shader creation success
	int success = GL_FALSE;
	// grab the shader source from the file
	char* source = Utility::fileToBuffer(a_filename);
	unsigned int shader = glCreateShader(a_type);
	// set the source buffer for the shader
	glShaderSource(shader, 1, (const char**)&source, 0);
	glCompileShader(shader);

	// test shader compilation fro any errors and display them to console
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (GL_FALSE == success)	// shader compilation failed, get logs and display them to console
	{
		int infoLogLength = 0; // Variable to store the length of the error log
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength]; // allocate buffer to hold data
		glGetShaderInfoLog(shader, infoLogLength, 0, infoLog);
		std::cout << "Unable to compile: " << a_filename << std::endl;
		std::cout << infoLog << std::endl;
		delete[] infoLog;
		return 0;
	}
	// Success - add shader to mShaders vector
	mShaders.push_back(shader);
	return shader;
}

void ShaderUtil::deleteShader(unsigned int a_shader)
{
	ShaderUtil* instance = ShaderUtil::GetInstance();
	instance->deleteShaderInternal(a_shader);
}

void ShaderUtil::deleteShaderInternal(unsigned int a_shader)
{
	// Loop through the shaders vector
	for (auto iter = mShaders.begin(); iter != mShaders.end(); iter++)
	{
		if (*iter == a_shader) // if we find the shader we are looking for
		{
			glDeleteShader(*iter); // delete the shader
			mShaders.erase(iter); // remove this item from the shaders vector
			break;
		}
	}
}

unsigned int ShaderUtil::createProgram(const int& a_vertexShader, const int& a_fragmentShader)
{
	ShaderUtil* instance = ShaderUtil::GetInstance();
	return instance->createProgramInternal(a_vertexShader, a_fragmentShader);
}

unsigned int ShaderUtil::createProgramInternal(const int& a_vertexShader, const int& a_fragmentShader)
{
	// boolean value to test for shader program linkage success
	int success = GL_FALSE;

	// Create a shader program and attatch the shaders to it
	unsigned int handle = glCreateProgram();
	glAttachShader(handle, a_vertexShader);
	glAttachShader(handle, a_fragmentShader);
	// Link the shaders together into one shader program
	glLinkProgram(handle);
	// Test to see if the program was successfully created
	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	// if something has gone wrong, execute the following
	if (GL_FALSE == success)
	{
		int infoLogLength = 0; // integer value to tellus the length of the error log
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		// Allocate enough spacein buffer for the error message
		char* infoLog = new char[infoLogLength];
		// Fill the buffer with data
		glGetProgramInfoLog(handle, infoLogLength, 0, infoLog);
		// Print log message to console
		std::cout << "Shader Linker Error" << std::endl;
		std::cout << infoLog << std::endl;

		// delete the char buffer now we have displayed it
		delete[] infoLog;
		return 0; // return 0, program id 0 is a null program
	}

	// add the program to the shader program vector
	mPrograms.push_back(handle);
	return handle;
}

void ShaderUtil::deleteProgram(unsigned int a_program)
{
	ShaderUtil* instance = ShaderUtil::GetInstance();
	instance->deleteProgramInternal(a_program);
}

void ShaderUtil::deleteProgramInternal(unsigned int a_program)
{
	// Loop through the shaders vector
	for (auto iter = mPrograms.begin(); iter != mPrograms.end(); iter++)
	{
		if (*iter == a_program) // if we find what we are looking for
		{
			glDeleteProgram(*iter); // delete the program
			mPrograms.erase(iter); // rtemove this item from the programs vector
			break;
		}
	}
}
