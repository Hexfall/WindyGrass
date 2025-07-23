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
    void InitializeMaterials();
    void RenderGUI();
    void RenderGrass();
    void RenderDirt();
    
    float planeWidth;
    float planeDepth;
    int grassDensity; // Blades of grass in a 1x1 area.

    DearImGui m_imGui;

    // Grass
    std::shared_ptr<ShaderProgram> m_grassShaderProgram;
    Mesh m_grassMesh;
    std::shared_ptr<Material> m_grassMaterial;
    
    // Dirt
    std::shared_ptr<ShaderProgram> m_dirtShaderProgram;
    Mesh m_dirtMesh;
    std::shared_ptr<Material> m_dirtMaterial;
    
    Camera m_camera;
    glm::vec3 m_cameraPos;
    glm::vec3 m_cameraDir;
    float m_cameraSpeed;
    
    glm::vec2 m_mousePos;

    float m_grassHeightToWidthRatio;
    float m_grassHeightToLengthRatio;
    glm::vec2 m_grassStalkPoint;
    glm::vec2 m_grassPullPoint;
    glm::vec2 m_grassEndPoint;
    float m_time;
    int  m_grassSegmentsValue;
    float m_ambientReflectionValue;
    float m_diffuseReflectionValue;
    float m_specularReflectionValue;
    float m_specularExponentValue;
    
    glm::vec2 m_windSway;
    glm::vec2 m_windOffset;
    glm::vec2 m_windDirection;
    float m_windSpeed;
    bool m_showWind;

    void UpdateCamera();

    std::shared_ptr<Texture2DObject> LoadTexture(const char *path);

    std::shared_ptr<Texture2DObject> CreatePerlinNoise(unsigned int width, unsigned int height, glm::ivec2 coords, bool brownian = false);
};
