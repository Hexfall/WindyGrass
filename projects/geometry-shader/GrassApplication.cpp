#include "GrassApplication.h"
#include "ituGL/geometry/VertexFormat.h"

#include <iostream>
#include <span>

#include <ituGL/shader/Shader.h>
#include <ituGL/asset/ShaderLoader.h>
#include <imgui.h>

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

GrassApplication::GrassApplication()
    : Application(1280, 720, "Geometry Shader"),
    m_grassHeightToWidthRatioValue(0.035f),
    m_grassHeightToLengthRatioValue(0.2),
    m_cameraPos(0),
    m_cameraDir(0, -.2, 1) {
}

void GrassApplication::Initialize() {
    Application::Initialize();
    
    InitializeShaders();
    UpdateCamera();

    m_imGui.Initialize(GetMainWindow());

    // Define the vertex structure
    struct Vertex
    {
        Vertex() = default;
        Vertex(const glm::vec3& position, const float height) : position(position), height(height) {}
        glm::vec3 position;
        float height;
    };
    
    VertexFormat grassVertexFormat;
    grassVertexFormat.AddVertexAttribute<float>(3);
    grassVertexFormat.AddVertexAttribute<float>(1);
    
    std::vector<Vertex> grassVertices;
    grassVertices.emplace_back(glm::vec3(0), 0.3f);
    grassVertices.emplace_back(glm::vec3(0.5, 0, 0), 0.45f);
    
    for (int i = 0; i < 100000; i++) {
        grassVertices.emplace_back(glm::vec3((float)rand() / RAND_MAX * 2 - 1, 0, (float)rand() / RAND_MAX * 2 - 1), (float)rand() / RAND_MAX*.15 + 0.15f);
    }
    
    m_grassMesh.AddSubmesh<Vertex, VertexFormat::LayoutIterator>(
            Drawcall::Primitive::Points,
            grassVertices,
            grassVertexFormat.LayoutBegin(
                    static_cast<int>(grassVertices.size()),
                    true /* interleaved */),
         grassVertexFormat.LayoutEnd()
        );

    m_grassHeightToWidthRatio = m_grassShaderProgram.GetUniformLocation("HeightToWidthRatio");
    m_grassHeightToLengthRatio = m_grassShaderProgram.GetUniformLocation("HeightToLengthRatio");
    m_grassSegments = m_grassShaderProgram.GetUniformLocation("Segments");
    m_viewProjMatrix = m_grassShaderProgram.GetUniformLocation("ViewProjMatrix");
    m_worldMatrix = m_grassShaderProgram.GetUniformLocation("WorldMatrix");
}

void GrassApplication::Update() {
    Application::Update();
    
    if (GetMainWindow().IsMouseButtonPressed(Window::MouseButton::Left)) {
        glm::vec2 mouseDelta = GetMainWindow().GetMousePosition(true) - m_mousePos;
        m_cameraDir = glm::rotate(mouseDelta.x * 0.5f, glm::vec3(0, 1, 0)) * glm::vec4(m_cameraDir, 0);
        m_cameraDir = glm::rotate(mouseDelta.y * 0.5f, glm::vec3(1, 0, 0)) * glm::vec4(m_cameraDir, 0);
    }
    
    m_mousePos = GetMainWindow().GetMousePosition(true);
    
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_W))
        m_cameraPos += m_cameraDir * 0.1f;
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_S))
        m_cameraPos -= m_cameraDir * 0.1f;
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_A))
        m_cameraPos -= glm::normalize(glm::cross(m_cameraDir, glm::vec3(0, 1, 0))) * 0.1f;
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_D))
        m_cameraPos += glm::normalize(glm::cross(m_cameraDir, glm::vec3(0, 1, 0))) * 0.1f;
    
    UpdateCamera();
}

void GrassApplication::UpdateCamera() {
    m_camera.SetViewMatrix(m_cameraPos, m_cameraPos+m_cameraDir, glm::vec3(0, 1, 0));
    float aspectRatio = GetMainWindow().GetAspectRatio();
    m_camera.SetPerspectiveProjectionMatrix(1.0f, aspectRatio, 0.1f, 1000.0f);
}

void GrassApplication::Render() {
    Application::Render();
    
    GetDevice().Clear(true, Color(0.195f, 0.598f, 0.797f, 1.0f), true, 1.0f);
    
    m_grassShaderProgram.Use();
    m_grassShaderProgram.SetUniform(m_worldMatrix, glm::scale(glm::vec3(1.0f)));
    m_grassShaderProgram.SetUniform(m_viewProjMatrix, m_camera.GetViewProjectionMatrix());
    m_grassShaderProgram.SetUniform(m_grassHeightToWidthRatio, m_grassHeightToWidthRatioValue);
    m_grassShaderProgram.SetUniform(m_grassHeightToLengthRatio, m_grassHeightToLengthRatioValue);
    m_grassShaderProgram.SetUniform(m_grassSegments, 16u);
    m_grassMesh.DrawSubmesh(0);
    
    RenderGUI();
}

void GrassApplication::RenderGUI() {
    m_imGui.BeginFrame();

    ImGui::DragFloat("Height to width ratio", &m_grassHeightToWidthRatioValue, 0.001f, 0.001f, 10.0f);
    ImGui::DragFloat("Height to length ratio", &m_grassHeightToLengthRatioValue, 0.001f, 0.001f, 10.0f);
    ImGui::DragFloat3("Camera Position", &m_cameraPos[0]);
    ImGui::DragFloat3("Camera Angle", &m_cameraDir[0]);
    
    m_imGui.EndFrame();
}

void GrassApplication::InitializeShaders() {
    // Vertex shader(s)
    Shader grassVertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/grass.vert");

    // Geometry shader(s)
    Shader grassGeoShader = ShaderLoader(Shader::GeometryShader).Load("shaders/grass.geom");

    // Geometry shader(s)
    Shader grassFragShader = ShaderLoader(Shader::FragmentShader).Load("shaders/grass.frag");
    
    if (!m_grassShaderProgram.Build(grassVertexShader, grassFragShader, grassGeoShader)) {
        char c[512];
        std::span<char> err(c);
        std::cerr << "Error linking shaders" << std::endl;
        m_grassShaderProgram.GetLinkingErrors(err);
        for (const auto& e : err) {
            std::cerr << e;
        }
        std::cerr << std::endl;
        return;
    }
}

void GrassApplication::Cleanup() {
    m_imGui.Cleanup();
    
    Application::Cleanup();
}
