#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../include/ApplicationEvent.h"
#include"../include/Dispatcher.h"

#include <iostream>
#include "../include/RenderViewport.h"


int main()
{
	RenderViewport* renderView = new RenderViewport();
	renderView->Run("Render ViewPort", 1920, 1080, false);
	delete renderView;
	return 0;
}
