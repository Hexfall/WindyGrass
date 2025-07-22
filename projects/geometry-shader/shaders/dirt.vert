#version 330

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 uvPosition;

out vec2 TexCoord;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;

void main() {
    TexCoord = uvPosition;
    gl_Position = ViewProjMatrix * WorldMatrix * vec4(vertexPosition, 1.0);
}