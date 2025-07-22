#version 330

in vec2 TexCoord;

uniform vec3 Color;
uniform bool ShowTexture;
uniform vec3 HiColor;
uniform vec3 LoColor;
uniform sampler2D PerlinTexture;

out vec4 FragColor;

void main() {
    FragColor = vec4(Color, 1.0);
}