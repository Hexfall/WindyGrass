#pragma once

#include <ituGL/application/Application.h>
#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/geometry/Mesh.h>
#include <memory>
#include "ituGL/utils/DearImGui.h"
#include "ituGL/camera/Camera.h"
#include "ituGL/shader/Material.h"
#include <ituGL/texture/Texture2DObject.h>

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
    void InitializeMesh();
    void InitializeMaterial();
    void RenderGUI();

    DearImGui m_imGui;
    
    std::shared_ptr<ShaderProgram> m_grassShaderProgram;
    Mesh m_grassMesh;
    std::shared_ptr<Material> m_grassMaterial;
    
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
    
    ShaderProgram::Location ml_cameraPos;
    ShaderProgram::Location ml_ambientReflection;
    ShaderProgram::Location ml_diffuseReflection;
    ShaderProgram::Location ml_specularReflection;
    ShaderProgram::Location ml_specularExponent;
    
    ShaderProgram::Location ml_grassColor;
    ShaderProgram::Location ml_stalkColor;
    ShaderProgram::Location ml_grassTexture;

    float mv_grassHeightToWidthRatioValue;
    float mv_grassHeightToLengthRatioValue;
    glm::vec2 mv_grassStalkPoint;
    glm::vec2 mv_grassPullPoint;
    glm::vec2 mv_grassEndPoint;
    float mv_time;
    int  mv_grassSegmentsValue;
    float mv_ambientReflectionValue;
    float mv_diffuseReflectionValue;
    float mv_specularReflectionValue;
    float mv_specularExponentValue;
    glm::vec3 mv_grassColorValue;
    glm::vec3 mv_stalkColorValue;
    std::shared_ptr<Texture2DObject> m_grassTexture;

    void UpdateCamera();

    std::shared_ptr<Texture2DObject> LoadTexture(const char *path);
};
