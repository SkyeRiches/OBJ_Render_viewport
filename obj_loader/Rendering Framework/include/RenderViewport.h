#pragma once

#include "../include/Application.h"
#include "../include/ApplicationEvent.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
// forward declare OBJ model
class OBJModel;

class RenderViewport : public Application
{
public:
	RenderViewport();
	virtual ~RenderViewport();

	void onWindowResize(WindowResizeEvent* e);

protected:
	virtual bool onCreate();
	virtual void Update(float deltaTime);
	virtual void Draw();
	virtual void Destroy();

private:
	// Structure for a simple vertex - interleaved (position, colour)
	typedef struct Vertex
	{
		glm::vec4 position;
		glm::vec4 colour;
	}Vertex;

	// Structure for a line
	typedef struct Line
	{
		Vertex v0;
		Vertex v1;
	}Line;

	glm::mat4 m_cameraMatrix;
	glm::mat4 m_projectionMatrix;

	unsigned int m_uiProgram;
	unsigned int m_objProgram;
	unsigned int m_lineVBO;
	unsigned int m_objModelBuffer[2];

	OBJModel* m_objModel;
	Line* m_lines;

	glm::vec3 m_backgroundColor;
};