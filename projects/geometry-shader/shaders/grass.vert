#version 330

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in float bladeHeight;

out float height;

void main() {
    height = bladeHeight;
    gl_Position = vec4(vertexPosition, 1.0);
}