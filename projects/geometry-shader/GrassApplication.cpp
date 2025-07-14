#include "GrassApplication.h"
#include "ituGL/geometry/VertexFormat.h"

#include <iostream>
#include <span>

#include <ituGL/shader/Shader.h>
#include <ituGL/asset/ShaderLoader.h>

GrassApplication::GrassApplication()
    : Application(1280, 720, "Geometry Shader") {
}

void GrassApplication::Initialize() {
    Application::Initialize();
    
    InitializeShaders();


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
    grassVertices.emplace_back(glm::vec3(0), 0.1f);
    grassVertices.emplace_back(glm::vec3(0.5, 0, 0), 0.2f);
    
    m_grassMesh.AddSubmesh<Vertex, VertexFormat::LayoutIterator>(
            Drawcall::Primitive::Points,
            grassVertices,
            grassVertexFormat.LayoutBegin(
                    static_cast<int>(grassVertices.size()),
                    true /* interleaved */),
         grassVertexFormat.LayoutEnd()
        );
    
    m_grassHeightToWidthRatio = m_grassShaderProgram.GetUniformLocation("HeightToWidthRatio");
}

void GrassApplication::Update() {
    Application::Update();
}

void GrassApplication::Render() {
    Application::Render();
    
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);
    
    m_grassShaderProgram.Use();
    m_grassShaderProgram.SetUniform(m_grassHeightToWidthRatio, 0.2f);
    m_grassMesh.DrawSubmesh(0);
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
