#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Camera
{
public:
	void FreeMovement(glm::mat4& a_transform, float a_deltaTime, float a_speed, const glm::vec3& a_up = glm::vec3(0, 1, 0));

};