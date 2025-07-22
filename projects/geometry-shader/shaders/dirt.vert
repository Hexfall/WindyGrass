#version 330

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 uvPosition;

out vec2 TexCoord;
out vec3 WorldPosition;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;

void main() {
    TexCoord = uvPosition;
    WorldPosition = (WorldMatrix * vec4(vertexPosition, 1.0)).xyz;
    gl_Position = ViewProjMatrix * vec4(WorldPosition, 1.0);
}