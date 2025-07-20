#version 330

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in float bladeHeight;
layout (location = 2) in float bladeAngle;
layout (location = 2) in float bladeOffset;

out float height;
out float angle;
out float timeOffset;

void main() {
    height = bladeHeight;
    angle = bladeAngle;
    timeOffset = bladeOffset;
    gl_Position = vec4(vertexPosition, 1.0);
}