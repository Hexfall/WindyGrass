#version 330

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in float bladeHeight;
layout (location = 2) in float bladeAngle;
layout (location = 2) in float bladeOffset;

out float height;
out float angle;
out float timeOffset;

uniform sampler2D HeightMap;

void main() {
    height = bladeHeight * (texture(HeightMap, vertexPosition.xz/10).r + .35) * 2;
    angle = bladeAngle;
    timeOffset = bladeOffset;
    gl_Position = vec4(vertexPosition, 1.0);
}