#include "../include/Application.h"
#include "../include/Utility.h"
#include "../include/ShaderUtil.h"
#include "../include/ApplicationEvent.h"

// Include the openGL header
#include <glad/glad.h>
// Include GLFW header
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

// include io stream for console logging
#include <iostream>

bool Application::Create(const char* a_applicationName, unsigned int a_windowWidth, unsigned int a_windowHeight, bool a_fullscreen)
{
	if (!glfwInit()) { return false; }

	m_windowWidth = a_windowWidth;
	m_windowHeight = a_windowHeight;
	// create a windowed mode window and its open gl context
	m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, a_applicationName, (a_fullscreen ? glfwGetPrimaryMonitor() : nullptr), nullptr);
	if (!m_window)
	{
		glfwTerminate();
		return false;
	}

	// make the window's context current
	glfwMakeContextCurrent(m_window);

	// Initialise GLAD - load in gl extensions
	if (!gladLoadGL())
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return false;
	}

	// Get the supported open gl version
	int major = glfwGetWindowAttrib(m_window, GLFW_CONTEXT_VERSION_MAJOR);
	int minor = glfwGetWindowAttrib(m_window, GLFW_CONTEXT_VERSION_MINOR);
	int revision = glfwGetWindowAttrib(m_window, GLFW_CONTEXT_REVISION);

	std::cout << "OpenGL Version " << major << "." << minor << "." << revision << std::endl;

	glfwSetWindowSizeCallback(m_window, [](GLFWwindow*, int w, int h)
	{
		// call the global dispatcher to handle this function
		Dispatcher* dp = Dispatcher::GetInstance();
		if (dp != nullptr)
		{
			dp->publish(new WindowResizeEvent(w, h), true);
		}
	});

	// creat dispatcher
	Dispatcher::CreateInstance();

	// Setup IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//Set IMGUI style
	ImGui::StyleColorsDark();
	const char* glsl_version = "#version 150";
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Implement a call on the derrived class onCreate function for any implementation specific code
	bool result = onCreate();
	if (result == false)
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}
	return result;
}

void Application::Run(const char* a_applicationName, unsigned int a_windowWidth, unsigned int a_windowHeight, bool a_fullscreen)
{
	if (Create(a_applicationName, a_windowWidth, a_windowHeight, a_fullscreen))
	{
		Utility::resetTimer();
		m_running = true;
		do
		{
			// start the imgui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			showFrameData(true);

			float deltaTime = Utility::tickTimer();
			Update(deltaTime);

			Draw();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(m_window);
			glfwPollEvents();
		} while (m_running == true && glfwWindowShouldClose(m_window) == 0);

		// cleanup imgui
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		Destroy();
		// Cleanup
		glfwDestroyWindow(m_window);
		glfwTerminate();

		Dispatcher::DestroyInstance();
	}
}

void Application::showFrameData(bool a_bShowFrameData) 
{
	// this function creates an imgui box that displays the frame time and the frame rate of the viewport
	const float DISTANCE = 10.0f;
	static int corner = 0;
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
	ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowBgAlpha(0.3f);
	
	if (ImGui::Begin("Frame Data", &a_bShowFrameData, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) 
	{
		ImGui::Separator();
		// display the frame time and the frame rate
		ImGui::Text("Application Average: %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		if (ImGui::IsMousePosValid()) 
		{
			// display the mouse position
			ImGui::Text("Mouse Position: (%.1f, %.1f)", io.MousePos.x, io.MousePos.y);
		}
		else
		{
			ImGui::Text("Mouse Position: <invalid>");
		}
	}
	ImGui::End();
}