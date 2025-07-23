#include "GrassApplication.h"
#include "ituGL/geometry/VertexFormat.h"

#include <iostream>
#include <span>

#include <ituGL/shader/Shader.h>
#include <ituGL/asset/ShaderLoader.h>
#include <imgui.h>

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GrassApplication::GrassApplication()
    : Application(1280, 720, "Geometry Shader"),
      m_grassHeightToWidthRatio(0.035f),
      m_grassHeightToLengthRatio(0.35f),
      m_cameraPos(0, 1.6, -2),
      m_cameraDir(0, -.2, 1),
      m_cameraSpeed(0.005),
      m_grassStalkPoint(0, 1.49),
      m_grassPullPoint(1.83, 6.06),
      m_grassEndPoint(6.11, 6.02),
      m_time(0.0f),
      m_grassSegmentsValue(16u),
      m_ambientReflectionValue(0.9),
      m_diffuseReflectionValue(0.9),
      m_specularReflectionValue(1.1),
      m_specularExponentValue(50.0),
      m_windOffset(0),
      m_windDirection(1, -1.63),
      m_windSpeed(.6),
      m_showWind(false),
      m_windSway(2, -2)
      {}

void GrassApplication::Initialize() {
    Application::Initialize();
    
    InitializeShaders();
    UpdateCamera();

    glEnable(GL_DEPTH_TEST);

    InitializeGrassMesh();
    InitializeDirtMesh();
    InitializeMaterial();

    m_imGui.Initialize(GetMainWindow());
}

void GrassApplication::InitializeShaders() {
    // Vertex shader(s)
    Shader grassVertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/grass.vert");
    Shader dirtVertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/dirt.vert");

    // Geometry shader(s)
    Shader grassGeoShader = ShaderLoader(Shader::GeometryShader).Load("shaders/grass.geom");

    // Geometry shader(s)
    Shader grassFragShader = ShaderLoader(Shader::FragmentShader).Load("shaders/grass.frag");
    Shader dirtFragShader = ShaderLoader(Shader::FragmentShader).Load("shaders/dirt.frag");

    // Build Shaders
    m_grassShaderProgram = std::make_shared<ShaderProgram>();
    if (!m_grassShaderProgram->Build(grassVertexShader, grassFragShader, grassGeoShader)) {
        char c[512];
        std::span<char> err(c);
        std::cerr << "Error linking shaders" << std::endl;
        m_grassShaderProgram->GetLinkingErrors(err);
        for (const auto& e : err) {
            std::cerr << e;
        }
        std::cerr << std::endl;
        return;
    }
    
    m_dirtShaderProgram = std::make_shared<ShaderProgram>();
    if (!m_dirtShaderProgram->Build(dirtVertexShader, dirtFragShader)) {
        char c[512];
        std::span<char> err(c);
        std::cerr << "Error linking shaders" << std::endl;
        m_grassShaderProgram->GetLinkingErrors(err);
        for (const auto& e : err) {
            std::cerr << e;
        }
        std::cerr << std::endl;
        return;
    }
}

void GrassApplication::InitializeGrassMesh() {
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

    for (int i = 0; i < 75000; i++) {
        grassVertices.emplace_back(
                glm::vec3(
                        ((float)rand() / RAND_MAX * 2 - 1)*10,
                        0,
                        ((float)rand() / RAND_MAX * 2 - 1)*10),
                (float)rand() / RAND_MAX*.35 + 0.25f,
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
}

void GrassApplication::InitializeDirtMesh() {
    // Define the vertex structure
    struct Vertex {
        Vertex() = default;
        Vertex(const glm::vec3& position, const glm::vec2& uvPosition) : position(position), uvPosition(uvPosition) {}
        glm::vec3 position;
        glm::vec2 uvPosition;
    };

    VertexFormat vertexFormat;
    vertexFormat.AddVertexAttribute<float>(3);
    vertexFormat.AddVertexAttribute<float>(2);

    std::vector<Vertex> vertices;
    vertices.emplace_back(glm::vec3(-10, 0, -10), glm::vec2(0, 0));
    vertices.emplace_back(glm::vec3(-10, 0, 10), glm::vec2(0, 1));
    vertices.emplace_back(glm::vec3(10, 0, -10), glm::vec2(1, 0));
    vertices.emplace_back(glm::vec3(10, 0, 10), glm::vec2(1, 1));

    m_dirtMesh.AddSubmesh<Vertex, VertexFormat::LayoutIterator>(
            Drawcall::Primitive::TriangleStrip,
            vertices,
            vertexFormat.LayoutBegin(
                    static_cast<int>(vertices.size()),
                    true /* interleaved */),
            vertexFormat.LayoutEnd()
    );
}

void GrassApplication::InitializeMaterial() {
    m_grassMaterial = std::make_shared<Material>(m_grassShaderProgram);

    auto grassTexture = LoadTexture("textures/grass.jpg");
    auto grassHeightMap = CreatePerlinNoise(1024, 1024, glm::ivec2(0, 0), true);
    auto grassWindMap = CreatePerlinNoise(1024, 1024, glm::ivec2(-1, -1));
    m_grassMaterial->SetUniformValue("GrassTexture", grassTexture);
    m_grassMaterial->SetUniformValue("HeightMap", grassHeightMap);
    m_grassMaterial->SetUniformValue("WindMap", grassWindMap);
    m_grassMaterial->SetBlendEquation(Material::BlendEquation::Add);
    m_grassMaterial->SetBlendParams(Material::BlendParam::SourceAlpha, Material::BlendParam::OneMinusSourceAlpha);
    
    m_dirtMaterial = std::make_shared<Material>(m_dirtShaderProgram);

    m_dirtMaterial->SetUniformValue("Color", glm::vec3(0.512, 0.395, 0.223));
    m_dirtMaterial->SetUniformValue("PerlinTexture", grassWindMap);
    m_dirtMaterial->SetUniformValue("HiColor", glm::vec3(1, 0, 0));
    m_dirtMaterial->SetUniformValue("LoColor", glm::vec3(0, 1, 0));
    m_dirtMaterial->SetUniformValue("ShowTexture", (m_showWind ? 1u : 0u));
    m_dirtMaterial->SetBlendEquation(Material::BlendEquation::Add);
    m_dirtMaterial->SetBlendParams(Material::BlendParam::SourceAlpha, Material::BlendParam::OneMinusSourceAlpha);
}

void GrassApplication::Update() {
    Application::Update();

    m_time += GetDeltaTime();
    
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
    
    m_windOffset += GetDeltaTime() * m_windSpeed * m_windDirection;
    
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
    
    RenderGrass();
    RenderDirt();
    
    RenderGUI();
}

void GrassApplication::RenderGrass() {
    m_grassMaterial->Use();
    // World information.
    m_grassMaterial->SetUniformValue("WorldMatrix", glm::scale(glm::vec3(1.0f)));
    m_grassMaterial->SetUniformValue("ViewProjMatrix", m_camera.GetViewProjectionMatrix());
    m_grassMaterial->SetUniformValue("CameraPosition", m_cameraPos);
    
    // Blade construction variables.
    m_grassMaterial->SetUniformValue("HeightToWidthRatio", m_grassHeightToWidthRatio);
    m_grassMaterial->SetUniformValue("HeightToLengthRatio", m_grassHeightToLengthRatio);
    m_grassMaterial->SetUniformValue("Segments", (unsigned int) m_grassSegmentsValue);
    m_grassMaterial->SetUniformValue("BezStalk", m_grassStalkPoint);
    m_grassMaterial->SetUniformValue("BezPull", m_grassPullPoint);
    m_grassMaterial->SetUniformValue("BezEnd", m_grassEndPoint);
    
    // Idle swaying values.
    m_grassMaterial->SetUniformValue("SwayAmount", glm::vec2(1, -1));
    m_grassMaterial->SetUniformValue("SwaySpeed", 1.0f);
    m_grassMaterial->SetUniformValue("Time", m_time);
    
    // Blinn-Phong properties.
    m_grassMaterial->SetUniformValue("AmbientReflection", m_ambientReflectionValue);
    m_grassMaterial->SetUniformValue("DiffuseReflection", m_diffuseReflectionValue);
    m_grassMaterial->SetUniformValue("SpecularReflection", m_specularReflectionValue);
    m_grassMaterial->SetUniformValue("SpecularExponent", m_specularExponentValue);
    m_grassMaterial->SetUniformValue("LightVector", glm::normalize(glm::vec3(.75, 1, 0)));
    m_grassMaterial->SetUniformValue("AmbientColor", glm::vec3(0.25));;
    m_grassMaterial->SetUniformValue("LightColor", glm::vec3(1));

    // Wind properties.
    m_grassMaterial->SetUniformValue("WindOffset", m_windOffset);
    m_grassMaterial->SetUniformValue("WindSway", m_windSway);

    m_grassMesh.DrawSubmesh(0);
}

void GrassApplication::RenderDirt() {
    m_dirtMaterial->Use();
    // World information.
    m_dirtMaterial->SetUniformValue("WorldMatrix", glm::scale(glm::vec3(1.0f)));
    m_dirtMaterial->SetUniformValue("ViewProjMatrix", m_camera.GetViewProjectionMatrix());
    
    // Wind map values.
    m_dirtMaterial->SetUniformValue("ShowTexture", (m_showWind ? 1u : 0u));
    m_dirtMaterial->SetUniformValue("TextureOffset", m_windOffset);
    
    m_dirtMesh.DrawSubmesh(0);
}

void GrassApplication::RenderGUI() {
    m_imGui.BeginFrame();

    if (ImGui::CollapsingHeader("Camera")) {
        ImGui::DragFloat("Camera Speed", &m_cameraSpeed, 0.001f, 0.0001f, 0.01f);
        ImGui::DragFloat3("Camera Position", &m_cameraPos[0]);
        ImGui::DragFloat3("Camera Angle", &m_cameraDir[0]);
    }
    if (ImGui::CollapsingHeader("Grass Properties")) {
        ImGui::DragFloat("Height to width ratio", &m_grassHeightToWidthRatio, 0.001f, 0.001f, 10.0f);
        ImGui::DragFloat("Height to length ratio", &m_grassHeightToLengthRatio, 0.001f, 0.001f, 10.0f);
        ImGui::DragFloat2("Stalk Point", &m_grassStalkPoint[0], 0.05f);
        ImGui::DragFloat2("Pull Point", &m_grassPullPoint[0], 0.05f);
        ImGui::DragFloat2("End Point", &m_grassEndPoint[0], 0.05f);
        ImGui::DragFloat("Time", &m_time, 0.05f);
        ImGui::DragInt("Segments", &m_grassSegmentsValue, 1, 1, 16);
    }
    if (ImGui::CollapsingHeader("Material Properties")) {
        ImGui::DragFloat("Ambient Reflection", &m_ambientReflectionValue, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Diffuse Reflection", &m_diffuseReflectionValue, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Specular Reflection", &m_specularReflectionValue, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Specular Exponent", &m_specularExponentValue, 0.1f, 0.0f, 100.0f);
    }
    if (ImGui::CollapsingHeader("Wind Properties")) {
        ImGui::Checkbox("Show Wind", &m_showWind);
        ImGui::DragFloat("Wind Speed", &m_windSpeed, 0.1f, 0.0001f, 10.0f);
        ImGui::DragFloat2("Wind Direction", &m_windDirection[0], 0.05f);
        ImGui::DragFloat2("Wind Sway", &m_windSway[0], 0.01f, -10.0f, 10.0f);
    }
    
    m_imGui.EndFrame();
}

void GrassApplication::Cleanup() {
    m_imGui.Cleanup();
    
    Application::Cleanup();
}

std::shared_ptr<Texture2DObject> GrassApplication::LoadTexture(const char* path) {
    std::shared_ptr<Texture2DObject> texture = std::make_shared<Texture2DObject>();

    int width = 0;
    int height = 0;
    int components = 0;

    // Load the texture data here
    unsigned char* data = stbi_load(path, &width, &height, &components, 4);

    texture->Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    texture->SetImage(0, width, height, TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA, std::span<const unsigned char>(data, width * height * 4));

    // Generate mipmaps
    texture->GenerateMipmap();

    // Release texture data
    stbi_image_free(data);

    return texture;
}

std::shared_ptr<Texture2DObject> GrassApplication::CreatePerlinNoise(unsigned int width, unsigned int height, glm::ivec2 coords, bool brownian) {
    std::shared_ptr<Texture2DObject> noise = std::make_shared<Texture2DObject>();

    std::vector<float> pixels(height * width);
    for (unsigned int j = 0; j < height; ++j)
    {
        for (unsigned int i = 0; i < width; ++i)
        {
            float x = static_cast<float>(i) / (width - 1) + coords.x;
            float y = static_cast<float>(j) / (height - 1) + coords.y;
            float noiseVal;
            if (brownian)
                noiseVal = stb_perlin_fbm_noise3(x, y, 0.0f, 2.0f, 0.5f, 4);
            else
                noiseVal = stb_perlin_noise3(x, y, 0.0f, 0.0f, 0.0f, 0.0f);
            noiseVal = (noiseVal + 1.0f) * 0.5f; // Normalize range to [0, 1].
            pixels[j * width + i] = noiseVal;
        }
    }

    noise->Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    noise->SetImage<float>(0, width, height, TextureObject::FormatR, TextureObject::InternalFormatR16F, pixels);
    noise->GenerateMipmap();

    return noise;
}
