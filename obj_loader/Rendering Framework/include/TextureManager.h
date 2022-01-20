#pragma once
#include <map>
#include <string>

// forward declare texture as we only need to keep a pointer here
// and this avoids cyclic dependancy
class Texture;

class TextureManager
{
public:
	// this manager class will act as a singleton objecct for ease of access

	static TextureManager* CreateInstance();
	static TextureManager* GetInstance();
	static void DestroyInstance();

	bool TextureExists(const char* a_pName);
	// load a texture from file --> calls texture::load()
	unsigned int LoadTexture(const char* a_pfileName);
	unsigned int GetTexture(const char* a_fileName);

	void ReleaseTexture(unsigned int a_texture);

private:
	static TextureManager* m_instance;

	// a small struct to reference count a texture
	// ref count indicates how many pointers there are
	typedef struct TextureRef
	{
		Texture* pTexture;
		unsigned int refCount;
	}TextureRef;

	std::map<std::string, TextureRef> m_pTextureMap;

	TextureManager();
	~TextureManager();
};