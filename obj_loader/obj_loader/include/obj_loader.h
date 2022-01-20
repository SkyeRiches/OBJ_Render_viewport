#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

// A basic vertex calss for an obj file, it supports vertex position, vertex normal. vertex uv coord
class OBJVertex
{
public:
	enum VertexAttributeFlags
	{
		POSITION	= (1 << 0),	// The position of the vertex
		NORMAL		= (1 << 1),	// the normal for th vertex
		UVCOORD		= (1 << 2),	// The uv coordinates for the vertex
	};

	enum Offsets
	{
		PositionOffset	= 0,
		NormalOffset	= PositionOffset + sizeof(glm::vec4),
		UVCoordOffset	= NormalOffset + sizeof(glm::vec4),
	};

	OBJVertex();
	~OBJVertex();

	glm::vec4 position, normal;
	glm::vec2 uvcoord;

	bool operator == (const OBJVertex& a_rhs) const;
	bool operator < (const OBJVertex& a_rhs) const;
};
// Inline constructor destructor for the objvertex class
inline OBJVertex::OBJVertex() : position(0, 0, 0, 1), normal(0, 0, 0, 0), uvcoord(0, 0) {}
inline OBJVertex::~OBJVertex() {}
// Inline comparitor methods for objvertex
inline bool OBJVertex::operator == (const OBJVertex& a_rhs) const
{
	return memcmp(this, &a_rhs, sizeof(OBJVertex)) == 0;
}

inline bool OBJVertex::operator < (const OBJVertex& a_rhs) const
{
	return memcmp(this, &a_rhs, sizeof(OBJVertex)) < 0;
}

// An OBJ material
// materials havve properties such as lights, textures, roughness
class OBJMaterial
{
public:
	OBJMaterial() : name(), kA(0.0f), kD(0.0f), kS(0.0f) {};
	~OBJMaterial() {};

	std::string name;
	// colour and illumination variables
	glm::vec4 kA;
	glm::vec4 kD;
	glm::vec4 kS;

	// enum for the texture our objmodel will support
	enum TextureTypes
	{
		DiffuseTexture = 0,
		SpecularTexture,
		NormalTexture,

		TextureTypes_Count
	};
	// Texture will have filenames for loading, once loaded id's stored in id array
	std::string textureFileNames[TextureTypes_Count];
	unsigned int textureIDs[TextureTypes_Count];
};

// An OBJ model can be composed of many meshes. much like any 3d model
// lets use a class to store individual mesh data
class OBJMesh
{
public:
	OBJMesh();
	~OBJMesh();

	glm::vec4 calculateFaceNormal(const unsigned int& a_indexA, const unsigned int& a_indexB, const unsigned int& a_indexC) const;
	void calculateFaceNormals();

	std::string m_name;
	std::vector<OBJVertex> m_vertices;
	std::vector<unsigned int> m_indices;
	OBJMaterial* m_material;
};
// Inline constructor destrucotr -- to be expanded upon as required
inline OBJMesh::OBJMesh() {}
inline OBJMesh::~OBJMesh() {};

class OBJModel
{
public:
	OBJModel() : m_worldMatrix(glm::mat4(1.0f)), m_path(), m_meshes() {};
	~OBJModel()
	{
		unload();	// Function to unload any data loaded in from file
	};

	// Load from file function
	bool load(const char* a_fileName, float a_scale = 1.0f);
	// Function to unload and free memory
	void unload();
	// Functions to retrieve path, number of meshes and world matrix of model
	const char* getPath() const { return m_path.c_str(); }
	unsigned int getMeshCount() const { return m_meshes.size(); }
	const glm::mat4& getWorldMatrix() const { return m_worldMatrix; }
	// Functions to retrieve mesh by name ord index for models that contain multiple meshes
	OBJMesh* getMeshByName(const char* a_name);
	OBJMesh* getMeshByIndex(unsigned int a_index);

	OBJMaterial* getMaterialByName(const char* a_name);
	OBJMaterial* getMaterialByIndex(unsigned int a_index);

	unsigned int getMaterialCount() const { return m_materials.size(); }

private:
	// Function to process line data read in from file
	std::string lineType(const std::string& a_in);
	std::string lineData(const std::string& a_in);
	glm::vec4 processVectorString(const std::string a_data);
	std::vector<std::string> splitStringAtCharacter(std::string data, char a_character);

	void LoadMaterialLibrary(std::string a_mtllib);

	// obj face triplet struct
	typedef struct obj_face_triplet
	{
		unsigned int v, vt, vn;
	}obj_face_triplet;
	// function to extract triplet data from obj file
	obj_face_triplet ProcessTriplet(std::string a_triplet);

	// vector to store mesh data
	std::vector<OBJMesh*> m_meshes;
	// path to model data - useful for things like texture lookups
	std::string m_path;
	// Root mat4 (world matrix)
	glm::mat4 m_worldMatrix;
	std::vector<OBJMaterial*> m_materials;
};