#pragma once
#include <string>

// a class to store texture data
// a texture is a data buffer that contains values which relate to pixel colours

class Texture
{
public:
	Texture();
	~Texture();

	// function to load a texture from a file
	bool Load(std::string a_fileName);
	void unload();
	//get filename
	const std::string& GetFileName() const { return m_fileName; }
	unsigned int GetTextureID() const { return m_textureID; }
	void GetDimensions(unsigned int& a_w, unsigned int& a_h) const;

private:
	std::string m_fileName;
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_textureID;
};

inline void Texture::GetDimensions(unsigned int& a_w, unsigned int& a_h) const
{
	a_w = m_width; a_h = m_height;
}