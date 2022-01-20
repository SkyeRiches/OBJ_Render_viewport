//#include "../include/Camera.h"
//
//void Camera::FreeMovement(glm::mat4& a_transform, float a_deltaTime, float a_speed, const glm::vec3& a_up)
//{
//	// Get the current window context
//	GLFWwindow* window = glfwGetCurrentContext();
//
//	// Get the camera's forward, right, up and location vectors
//	glm::vec4 vForward = a_transform[2];
//	glm::vec4 vRight = a_transform[0];
//	glm::vec4 vUp = a_transform[1];
//	glm::vec4 vTranslation = a_transform[3];
//	// Test to see if the left shft key is pressed
//	// We will use left shift to double the speed of the camera movement
//	float frameSpeed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? a_deltaTime * a_speed * 2 : a_deltaTime * a_speed;
//
//	// Translate camera
//	// W & S forward and back
//	// A & D left and right
//	// Q and E - up and down (vertical displacement)
//	if (glfwGetKey(window, 'W') == GLFW_PRESS)
//	{
//		vTranslation -= vForward * frameSpeed;
//	}
//	if (glfwGetKey(window, 'S') == GLFW_PRESS)
//	{
//		vTranslation += vForward * frameSpeed;
//	}
//	if (glfwGetKey(window, 'D') == GLFW_PRESS)
//	{
//		vTranslation += vRight * frameSpeed;
//	}
//	if (glfwGetKey(window, 'A') == GLFW_PRESS)
//	{
//		vTranslation -= vRight * frameSpeed;
//	}
//	if (glfwGetKey(window, 'Q') == GLFW_PRESS)
//	{
//		vTranslation += vUp * frameSpeed;
//	}
//	if (glfwGetKey(window, 'E') == GLFW_PRESS)
//	{
//		vTranslation -= vUp * frameSpeed;
//	}
//	// Set the translation to the camera matrix that has been passed in.
//	a_transform[3] = vTranslation;
//
//	// Check for camera rotation
//	// Test for mouse button being held/pressed for rotation (button 2)
//	static bool sbMouseButtonDown = false;
//	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
//	{
//		static double siPrevMouseX = 0;
//		static double siPrevMouseY = 0;
//
//		if (sbMouseButtonDown == false)
//		{
//			sbMouseButtonDown = true;
//			glfwGetCursorPos(window, &siPrevMouseX, &siPrevMouseY);
//		}
//
//		double mouseX = 0, mouseY = 0;
//		glfwGetCursorPos(window, &mouseX, &mouseY);
//
//		double iDeltaX = mouseX - siPrevMouseX;
//		double iDeltaY = mouseY - siPrevMouseY;
//
//		siPrevMouseX = mouseX;
//		siPrevMouseY = mouseY;
//
//		glm::mat4 mMat;
//
//		// Pitch
//		if (iDeltaY != 0)
//		{
//			mMat = glm::axisAngleMatrix(vRight.xyz(), (float)-iDeltaY / 150.0f);
//			vRight = mMat * vRight;
//			vUp = mMat * vUp;
//			vForward = mMat * vForward;
//		}
//
//		// Yaw
//		if (iDeltaX != 0)
//		{
//			mMat = glm::axisAngleMatrix(a_up, (float)-iDeltaX / 150.0f);
//			vRight = mMat * vRight;
//			vUp = mMat * vUp;
//			vForward = mMat * vForward;
//		}
//
//		a_transform[0] = vRight;
//		a_transform[1] = vUp;
//		a_transform[2] = vForward;
//	}
//	else
//	{
//		sbMouseButtonDown = false;
//	}
//	
//}