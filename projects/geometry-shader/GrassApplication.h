#pragma once

#include <ituGL/application/Application.h>
#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/geometry/Mesh.h>
#include "ituGL/utils/DearImGui.h"
#include "ituGL/camera/Camera.h"

class GrassApplication : public Application{
public:
    GrassApplication();
    
protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;
    
private:
    void InitializeShaders();
    void RenderGUI();

    DearImGui m_imGui;
    
    ShaderProgram m_grassShaderProgram;
    Mesh m_grassMesh;
    
    Camera m_camera;
    glm::vec3 m_cameraPos;
    glm::vec3 m_cameraDir;
    
    glm::vec2 m_mousePos;
    
    ShaderProgram::Location m_grassHeightToWidthRatio;
    ShaderProgram::Location m_grassHeightToLengthRatio;
    ShaderProgram::Location m_grassSegments;
    ShaderProgram::Location m_viewProjMatrix;

    ShaderProgram::Location m_worldMatrix;

    float m_grassHeightToWidthRatioValue;
    float m_grassHeightToLengthRatioValue;

    void UpdateCamera();
};
