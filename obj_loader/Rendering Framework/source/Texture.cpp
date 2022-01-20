#include "../include/Texture.h"
#include <stb_image.h>
#include <iostream>
#include <glad/glad.h>

Texture::Texture() : m_fileName(), m_width(0), m_height(0), m_textureID(0)
{
}

Texture::~Texture()
{
	unload();
}

bool Texture::Load(std::string a_filepath)
{
	int width = 0, height = 0, channels = 0;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* imageData = stbi_load(a_filepath.c_str(), &width, &height, &channels, 4);
	if (imageData != nullptr)
	{
		m_fileName = a_filepath;
		m_width = width;
		m_height = height;
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(imageData);
		std::cout << "Successfully loaded image file: " << a_filepath << std::endl;
		return true;
	}
	std::cout << "Failed to open image file: " << a_filepath << std::endl;
	return false;
}

void Texture::unload()
{
	glDeleteTextures(1, &m_textureID);
}
