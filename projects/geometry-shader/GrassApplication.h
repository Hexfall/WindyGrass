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
    float m_cameraSpeed;
    
    glm::vec2 m_mousePos;
    
    // memberlocation_name
    ShaderProgram::Location ml_grassHeightToWidthRatio;
    ShaderProgram::Location ml_grassHeightToLengthRatio;
    ShaderProgram::Location ml_grassSegments;
    ShaderProgram::Location ml_grassStalkPoint;
    ShaderProgram::Location ml_grassPullPoint;
    ShaderProgram::Location ml_grassEndPoint;
    ShaderProgram::Location ml_time;
    
    ShaderProgram::Location ml_viewProjMatrix;
    ShaderProgram::Location ml_worldMatrix;

    float mv_grassHeightToWidthRatioValue;
    float mv_grassHeightToLengthRatioValue;
    glm::vec2 mv_grassStalkPoint;
    glm::vec2 mv_grassPullPoint;
    glm::vec2 mv_grassEndPoint;
    float mv_time;
    int  mv_grassSegmentsValue;

    void UpdateCamera();
};
