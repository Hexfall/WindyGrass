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
    void InitializeGrassMesh();
    void InitializeDirtMesh();
    void InitializeMaterial();
    void RenderGUI();

    DearImGui m_imGui;

    std::shared_ptr<ShaderProgram> m_grassShaderProgram;
    std::shared_ptr<ShaderProgram> m_dirtShaderProgram;
    Mesh m_grassMesh;
    Mesh m_dirtMesh;
    std::shared_ptr<Material> m_grassMaterial;
    std::shared_ptr<Material> m_dirtMaterial;
    
    Camera m_camera;
    glm::vec3 m_cameraPos;
    glm::vec3 m_cameraDir;
    float m_cameraSpeed;
    
    glm::vec2 m_mousePos;

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
    std::shared_ptr<Texture2DObject> m_grassTexture;
    
    glm::vec2 m_windSway;
    glm::vec2 m_windOffset;
    glm::vec2 m_windDirection;
    float m_windSpeed;
    bool m_showWind;

    void UpdateCamera();

    std::shared_ptr<Texture2DObject> LoadTexture(const char *path);

    std::shared_ptr<Texture2DObject> CreateBrownianPerlinNoise(unsigned int width, unsigned int height, glm::ivec2 coords);

    std::shared_ptr<Texture2DObject> CreatePerlinNoise(unsigned int width, unsigned int height, glm::ivec2 coords);
};
