#pragma once

#include <ituGL/application/Application.h>
#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/geometry/Mesh.h>

class GrassApplication : public Application{
public:
    GrassApplication();
    
protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    
private:
    void InitializeShaders();
    
    ShaderProgram m_grassShaderProgram;
    Mesh m_grassMesh;
    ShaderProgram::Location m_grassHeightToWidthRatio;
};
