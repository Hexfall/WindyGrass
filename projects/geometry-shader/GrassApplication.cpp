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
      mv_grassHeightToWidthRatioValue(0.035f),
      mv_grassHeightToLengthRatioValue(0.3),
      m_cameraPos(0, .6, -2),
      m_cameraDir(0, -.2, 1),
      m_cameraSpeed(0.005),
      mv_grassStalkPoint(0, 1.49),
      mv_grassPullPoint(1.83, 6.06), 
      mv_grassEndPoint(6.11, 6.02),
      mv_time(0.0f),
      mv_grassSegmentsValue(16u),
      mv_ambientReflectionValue(1.0),
      mv_diffuseReflectionValue(1.0),
      mv_specularReflectionValue(1.0),
      mv_specularExponentValue(10.0)
      {}

void GrassApplication::Initialize() {
    Application::Initialize();
    
    InitializeShaders();
    UpdateCamera();

    glEnable(GL_DEPTH_TEST);

    m_imGui.Initialize(GetMainWindow());

    // Define the vertex structure
    struct Vertex
    {
        Vertex() = default;
        Vertex(const glm::vec3& position, const float height, const float angle) : position(position), height(height), angle(angle) { timeOffset = (float)rand() / RAND_MAX * 3.14159f; }
        glm::vec3 position;
        float height;
        float angle;
        float timeOffset;
    };
    
    VertexFormat grassVertexFormat;
    grassVertexFormat.AddVertexAttribute<float>(3);
    grassVertexFormat.AddVertexAttribute<float>(1);
    grassVertexFormat.AddVertexAttribute<float>(1);
    grassVertexFormat.AddVertexAttribute<float>(1);
    
    std::vector<Vertex> grassVertices;
    grassVertices.emplace_back(glm::vec3(0), 0.3f, 0.0);
    grassVertices.emplace_back(glm::vec3(0.5, 0, 0), 0.45f, 0.0);
    
    for (int i = 0; i < 10000; i++) {
        grassVertices.emplace_back(
                glm::vec3(
                        ((float)rand() / RAND_MAX * 2 - 1)*3.5,
                        0,
                        ((float)rand() / RAND_MAX * 2 - 1)*3.5),
                (float)rand() / RAND_MAX*.25 + 0.15f,
                (float)rand() / RAND_MAX * 2 * 3.14159f);
    }
    
    m_grassMesh.AddSubmesh<Vertex, VertexFormat::LayoutIterator>(
            Drawcall::Primitive::Points,
            grassVertices,
            grassVertexFormat.LayoutBegin(
                    static_cast<int>(grassVertices.size()),
                    true /* interleaved */),
         grassVertexFormat.LayoutEnd()
        );

    ml_grassHeightToWidthRatio = m_grassShaderProgram.GetUniformLocation("HeightToWidthRatio");
    ml_grassHeightToLengthRatio = m_grassShaderProgram.GetUniformLocation("HeightToLengthRatio");
    ml_grassSegments = m_grassShaderProgram.GetUniformLocation("Segments");
    ml_viewProjMatrix = m_grassShaderProgram.GetUniformLocation("ViewProjMatrix");
    ml_worldMatrix = m_grassShaderProgram.GetUniformLocation("WorldMatrix");
    ml_grassStalkPoint = m_grassShaderProgram.GetUniformLocation("BezStalk");
    ml_grassPullPoint = m_grassShaderProgram.GetUniformLocation("BezPull");
    ml_grassEndPoint = m_grassShaderProgram.GetUniformLocation("BezEnd");
    ml_time = m_grassShaderProgram.GetUniformLocation("Time");
    ml_cameraPos = m_grassShaderProgram.GetUniformLocation("CameraPosition");
    ml_ambientReflection = m_grassShaderProgram.GetUniformLocation("AmbientReflection");
    ml_diffuseReflection = m_grassShaderProgram.GetUniformLocation("DiffuseReflection");
    ml_specularReflection = m_grassShaderProgram.GetUniformLocation("SpecularReflection");
    ml_specularExponent = m_grassShaderProgram.GetUniformLocation("SpecularExponent");
}

void GrassApplication::Update() {
    Application::Update();
    
    mv_time += GetDeltaTime();
    
    if (GetMainWindow().IsMouseButtonPressed(Window::MouseButton::Right)) {
        glm::vec2 mouseDelta = GetMainWindow().GetMousePosition(true) - m_mousePos;
        m_cameraDir = glm::rotate(mouseDelta.x * 0.5f, glm::vec3(0, 1, 0)) * glm::vec4(m_cameraDir, 0);
        m_cameraDir = glm::rotate(mouseDelta.y * 0.5f, glm::vec3(1, 0, 0)) * glm::vec4(m_cameraDir, 0);
    }
    
    m_mousePos = GetMainWindow().GetMousePosition(true);
    
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_W))
        m_cameraPos += m_cameraDir * m_cameraSpeed;
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_S))
        m_cameraPos -= m_cameraDir * m_cameraSpeed;
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_A))
        m_cameraPos -= glm::normalize(glm::cross(m_cameraDir, glm::vec3(0, 1, 0))) * m_cameraSpeed;
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_D))
        m_cameraPos += glm::normalize(glm::cross(m_cameraDir, glm::vec3(0, 1, 0))) * m_cameraSpeed;
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_SPACE))
        m_cameraPos += glm::vec3(0, 1, 0) * m_cameraSpeed;
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
        m_cameraPos -= glm::vec3(0, 1, 0) * m_cameraSpeed;
    
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
    m_grassShaderProgram.SetUniform(ml_worldMatrix, glm::scale(glm::vec3(1.0f)));
    m_grassShaderProgram.SetUniform(ml_viewProjMatrix, m_camera.GetViewProjectionMatrix());
    m_grassShaderProgram.SetUniform(ml_grassHeightToWidthRatio, mv_grassHeightToWidthRatioValue);
    m_grassShaderProgram.SetUniform(ml_grassHeightToLengthRatio, mv_grassHeightToLengthRatioValue);
    m_grassShaderProgram.SetUniform(ml_grassSegments, (uint) mv_grassSegmentsValue);
    m_grassShaderProgram.SetUniform(ml_grassStalkPoint, mv_grassStalkPoint);
    m_grassShaderProgram.SetUniform(ml_grassPullPoint, mv_grassPullPoint);
    m_grassShaderProgram.SetUniform(ml_grassEndPoint, mv_grassEndPoint);
    m_grassShaderProgram.SetUniform(ml_time, mv_time);
    m_grassShaderProgram.SetUniform(ml_cameraPos, m_cameraPos);
    
    m_grassShaderProgram.SetUniform(ml_ambientReflection, mv_ambientReflectionValue);
    m_grassShaderProgram.SetUniform(ml_diffuseReflection, mv_diffuseReflectionValue);
    m_grassShaderProgram.SetUniform(ml_specularReflection, mv_specularReflectionValue);
    m_grassShaderProgram.SetUniform(ml_specularExponent, mv_specularExponentValue);
    
    m_grassMesh.DrawSubmesh(0);
    
    RenderGUI();
}

void GrassApplication::RenderGUI() {
    m_imGui.BeginFrame();

    if (ImGui::CollapsingHeader("Camera")) {
        ImGui::DragFloat("Camera Speed", &m_cameraSpeed, 0.001f, 0.0001f, 0.01f);
        ImGui::DragFloat3("Camera Position", &m_cameraPos[0]);
        ImGui::DragFloat3("Camera Angle", &m_cameraDir[0]);
    }
    if (ImGui::CollapsingHeader("Grass Properties")) {
        ImGui::DragFloat("Height to width ratio", &mv_grassHeightToWidthRatioValue, 0.001f, 0.001f, 10.0f);
        ImGui::DragFloat("Height to length ratio", &mv_grassHeightToLengthRatioValue, 0.001f, 0.001f, 10.0f);
        ImGui::DragFloat2("Stalk Point", &mv_grassStalkPoint[0], 0.05f);
        ImGui::DragFloat2("Pull Point", &mv_grassPullPoint[0], 0.05f);
        ImGui::DragFloat2("End Point", &mv_grassEndPoint[0], 0.05f);
        ImGui::DragFloat("Time", &mv_time, 0.05f);
        ImGui::DragInt("Segments", &mv_grassSegmentsValue, 1, 1, 16);
    }
    if (ImGui::CollapsingHeader("Material Properties")) {
        ImGui::DragFloat("Ambient Reflection", &mv_ambientReflectionValue, 0.001f, 0.0f, 1.0f);
        ImGui::DragFloat("Diffuse Reflection", &mv_diffuseReflectionValue, 0.001f, 0.0f, 1.0f);
        ImGui::DragFloat("Specular Reflection", &mv_specularReflectionValue, 0.001f, 0.0f, 1.0f);
        ImGui::DragFloat("Specular Exponent", &mv_specularExponentValue, 0.1f, 0.0f, 100.0f);
    }
    
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
