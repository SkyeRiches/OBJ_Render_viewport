# OBJ-Render-Viewport
This project was developed as part of a university assignment. The aim of the assignment was to use a combination of external libraries with C++ in order to create a 3D render window that allows a user to view OBJ model files.
As well as loading OBJ files, this system can apply textures to the model so the user can see what the model looks like with the texture on it.


To use this project the user needs to put a folder with the OBJ file and any material files into the following path:
OBJ_Render_viewport/obj_loader/Rendering Framework/resource/models/

Due to time limitations the project currently only displays the hard coded OBJ file so to display a different OBJ file the user would need to change the file name on line 91 of RenderViewport.cpp


Future development on the project may involve taking in a file name from the command line that loads before the window opens so that the user can easily choose what model they want to load.


# External-Libraries-Used

GLAD - https://glad.dav1d.de/
GLFW - https://www.glfw.org/
GLM - https://github.com/g-truc/glm
IMGUI - https://github.com/ocornut/imgui
STB - https://github.com/nothings/stb
