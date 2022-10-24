#include "../include/RenderViewport.h"
#include "../include/ShaderUtil.h"
#include "../include/Utility.h"
#include "../include/TextureManager.h"
#include "obj_loader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>

#include <iostream>


RenderViewport::RenderViewport()
{

}

RenderViewport::~RenderViewport()
{

}

bool RenderViewport::onCreate()
{
    Dispatcher* dp = Dispatcher::GetInstance();
    if (dp)
    {
        dp->subscribe(this, &RenderViewport::onWindowResize);
    }

    // get an instance of Texture manager
    TextureManager::CreateInstance();

    // Set the clear color and enable depth testing and backface culling
    glClearColor(0.25f, 0.45f, 0.75f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Create shader program
    unsigned int vertexShader = ShaderUtil::loadShader("resource/shaders/vertex.glsl", GL_VERTEX_SHADER);
    unsigned int fragmentShader = ShaderUtil::loadShader("resource/shaders/fragment.glsl", GL_FRAGMENT_SHADER);
    m_uiProgram = ShaderUtil::createProgram(vertexShader, fragmentShader);

    // Create a grid of lines to be drawn during our update
    // Create a 10x10 square grid
    m_lines = new Line[42];

    for (int i = 0, j = 0; i < 21; i++, j += 2)
    {
        m_lines[j].v0.position = glm::vec4(-10 + i, 0.0f, 10.0f, 1.0f);
        m_lines[j].v0.colour = (i == 10) ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        m_lines[j].v1.position = glm::vec4(-10 + i, 0.0f, -10.0f, 1.0f);
        m_lines[j].v1.colour = (i == 10) ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        
        m_lines[j + 1].v0.position = glm::vec4(10, 0.0f, -10.0f + i, 1.0f);
        m_lines[j + 1].v0.colour = (i == 10) ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        m_lines[j + 1].v1.position = glm::vec4(-10, 0.0f, -10.0f + i, 1.0f);
        m_lines[j + 1].v1.colour = (i == 10) ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    // Create a vertex buffer to hold our line data
    glGenBuffers(1, &m_lineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
    // Fill vertex buffer with line data
    glBufferData(GL_ARRAY_BUFFER, 42 * sizeof(Line), m_lines, GL_STATIC_DRAW);

    // As we have sent the line data to the gpu we no longer require it on the cpu side of memory
    
    // Enable the vertex array state, since we're sending in an array of vertices
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Specify where our vertex array is, how many components each vertex has,
    // the data type of each component and whether the data is normalised or not
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0) + 16);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_backgroundColor = glm::vec3(0.5f, 0.0f, 0.9f);
    glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, 1.0f);

    // Create a world-space matrix for a camera
    m_cameraMatrix = glm::inverse(glm::lookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

    // Create a perspective projection matrix with a 90 degree fov and widescreen aspect ratio
    m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, m_windowWidth / (float)m_windowHeight, 0.1f, 1000.0f);

    m_objModel = new OBJModel();
    if (m_objModel->load("resource/models/Model_C1102056/C1102056.obj", 0.1f))
    {
        TextureManager* pTM = TextureManager::GetInstance();
        // load in texture for model if any are present
        for (int i = 0; i < m_objModel->getMaterialCount(); i++)
        {
            OBJMaterial* mat = m_objModel->getMaterialByIndex(i);
            for (int n = 0; n < OBJMaterial::TextureTypes::TextureTypes_Count; n++)
            {
                if (mat->textureFileNames[n].size() > 0)
                {
                    unsigned int textureId = pTM->LoadTexture(mat->textureFileNames[n].c_str());
                    mat->textureIDs[n] = textureId;
                }
            }
        }

        // Setup Shaders for OBJ model rendering
        // Create OBJ shader Program
        unsigned int obj_vertexShader = ShaderUtil::loadShader("resource/shaders/obj_vertex.glsl", GL_VERTEX_SHADER);
        unsigned int obj_fragmentShader = ShaderUtil::loadShader("resource/shaders/obj_fragment.glsl", GL_FRAGMENT_SHADER);
        m_objProgram = ShaderUtil::createProgram(obj_vertexShader, obj_fragmentShader);
        // set up vertex and index buffer OBJ rendering
        glGenBuffers(2, m_objModelBuffer);
        // set upd vertex buffer data
        glBindBuffer(GL_ARRAY_BUFFER, m_objModelBuffer[0]);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
        return false;
    }

    return true;
}

void RenderViewport::Update(float deltaTime)
{
    Utility::freeMovement(m_cameraMatrix, deltaTime, 0.01f);

    // Set up an IMGUI window to control BG color
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 window_size = ImVec2(400.0f, 100.0f);
    ImVec2 window_pos = ImVec2(io.DisplaySize.x * 0.01f, io.DisplaySize.y * 0.05f);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
    if (ImGui::Begin("Set Background Color"))
    {
        ImGui::ColorEdit3("Background Color:", glm::value_ptr(m_backgroundColor));
    }
    ImGui::End();
}

void RenderViewport::Draw()
{
    glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, 1.0f);

    // Clear the back buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get the view matrix from the world-space camera matrix
    glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);
    glm::mat4 projectionViewMatrix = m_projectionMatrix * viewMatrix;

    // Enable Shaders
    glUseProgram(m_uiProgram);
    // Send the projection matrix to the vertex shader
    // Ask the shader program for the location of the projection view matrix uniform variable
    int projectionViewUniformLocation = glGetUniformLocation(m_uiProgram, "ProjectionViewMatrix");
    // Send this location a pointer to our glm::mat4 (send across float data)
    glUniformMatrix4fv(projectionViewUniformLocation, 1, false, glm::value_ptr(projectionViewMatrix));

    glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
    glBufferData(GL_ARRAY_BUFFER, 42 * sizeof(Line), m_lines, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Specifiy where our vertex array is, how many components each vertex has,
    // the data type of each component and whether the data is normalised or not
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0) + 16);

    glDrawArrays(GL_LINES, 0, 42 * 2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(0);

    glUseProgram(m_objProgram);
    // Set the projection view matirx for this shader
    projectionViewUniformLocation = glGetUniformLocation(m_objProgram, "ProjectionViewMatrix");
    // send this location a pointer to our glm::mat4 (send accross float data)
    glUniformMatrix4fv(projectionViewUniformLocation, 1, false, glm::value_ptr(projectionViewMatrix));

    for (int i = 0; i < m_objModel->getMeshCount(); i++)
    {
        // get the model matrix location from the shader program
        int modelMatrixUniformLocation = glGetUniformLocation(m_objProgram, "ModelMatrix");
        // send the obj model's world matrix data across to the shader program
        glUniformMatrix4fv(modelMatrixUniformLocation, 1, false, glm::value_ptr(m_objModel->getWorldMatrix()));

        int cameraPositionUniformLocation = glGetUniformLocation(m_objProgram, "camPos");
        glUniform4fv(cameraPositionUniformLocation, 1, glm::value_ptr(m_cameraMatrix[3]));

        OBJMesh* pMesh = m_objModel->getMeshByIndex(i);

        // send material data to shader
        int kA_location = glGetUniformLocation(m_objProgram, "kA");
        int kD_location = glGetUniformLocation(m_objProgram, "kD");
        int kS_location = glGetUniformLocation(m_objProgram, "kS");

        OBJMaterial* pMaterial = pMesh->m_material;
        if (pMaterial != nullptr)
        {
            // send the obj models world matrix data across to the shader program
            glUniform4fv(kA_location, 1, glm::value_ptr(pMaterial->kA));
            glUniform4fv(kD_location, 1, glm::value_ptr(pMaterial->kD));
            glUniform4fv(kS_location, 1, glm::value_ptr(pMaterial->kS));

            // get the location of the diffuse texture
            int texUniformLoc = glGetUniformLocation(m_objProgram, "DiffuseTexture");
            glUniform1i(texUniformLoc, 0); // set diffuse texture to be GL_Texture0

            glActiveTexture(GL_TEXTURE0);
            // bind the texture for diffuse for this material to the texture0
            glBindTexture(GL_TEXTURE_2D, pMaterial->textureIDs[OBJMaterial::TextureTypes::DiffuseTexture]);

            texUniformLoc = glGetUniformLocation(m_objProgram, "SpecularTexture");
            glUniform1i(texUniformLoc, 1); // set diffuse texture to be GL_Texture0

            glActiveTexture(GL_TEXTURE1);
            // bind the texture for specular for this material to the texture1
            glBindTexture(GL_TEXTURE_2D, pMaterial->textureIDs[OBJMaterial::TextureTypes::SpecularTexture]);

            texUniformLoc = glGetUniformLocation(m_objProgram, "NormalTexture");
            glUniform1i(texUniformLoc, 2); // set diffuse texture to be GL_Texture0

            glActiveTexture(GL_TEXTURE2);
            // bind the texture for diffuse for this material to the texture0
            glBindTexture(GL_TEXTURE_2D, pMaterial->textureIDs[OBJMaterial::TextureTypes::NormalTexture]);
        }
        else
        {
            glUniform4fv(kA_location, 1, glm::value_ptr(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f)));
            glUniform4fv(kD_location, 1, glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
            glUniform4fv(kS_location, 1, glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 64.0f)));
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_objModelBuffer[0]);
        glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(OBJVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_objModelBuffer[1]);
        glEnableVertexAttribArray(0); // position
        glEnableVertexAttribArray(1); // normal
        glEnableVertexAttribArray(2); // uv coord

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), ((char*)0) + OBJVertex::PositionOffset);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(OBJVertex), ((char*)0) + OBJVertex::NormalOffset);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(OBJVertex), ((char*)0) + OBJVertex::UVCoordOffset);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_indices.size() * sizeof(unsigned int), pMesh->m_indices.data(), GL_STATIC_DRAW);
        glDrawElements(GL_TRIANGLES, pMesh->m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glUseProgram(0);
}

void RenderViewport::Destroy()
{
    delete m_objModel;
    delete[] m_lines;
    glDeleteBuffers(1, &m_lineVBO);
    ShaderUtil::deleteProgram(m_uiProgram);
    TextureManager::DestroyInstance();
    ShaderUtil::DestroyInstance();
}

void RenderViewport::onWindowResize(WindowResizeEvent* e)
{
    std::cout << "Member event handler called" << std::endl;
    if (e->GetWidth() > 0 && e->GetHeight() > 0)
    {
        // Create a perspective projection matrix with a 90 degree fov and widescreen aspect ratio
        m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, e->GetWidth() / (float)e->GetHeight(), 0.1f, 1000.0f);
        glViewport(0, 0, e->GetWidth(), e->GetHeight());
    }
    e->Handled();
}