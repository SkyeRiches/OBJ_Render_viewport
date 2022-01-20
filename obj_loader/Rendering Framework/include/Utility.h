#pragma once
#include <glm/glm.hpp>

// A utility class with static helper methods
class Utility
{
public:
	// utilities for timing; Get() updates timers and returns time since last get call
	static void resetTimer();
	static float tickTimer();
	static float getDeltaTime();
	static float getTotalTime();

	// Helper function for loading shader code into memory
	static char* fileToBuffer(const char* a_szPath);

	// Utilitiy for mouse / keyboard movement of a matirx transform (suitable for camera)
	static void freeMovement(glm::mat4& a_transform, float a_deltaTime, float a_speed, const glm::vec3& a_up = glm::vec3(0, 1, 0));

};