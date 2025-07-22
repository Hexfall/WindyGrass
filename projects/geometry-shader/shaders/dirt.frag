#version 330

in vec2 TexCoord;
in vec3 WorldPosition;

uniform vec3 Color;
uniform uint ShowTexture;
uniform vec3 HiColor;
uniform vec3 LoColor;
uniform sampler2D PerlinTexture;
uniform vec2 TextureOffset;

out vec4 FragColor;

void main() {
    if (ShowTexture == 0u) {
        FragColor = vec4(Color, 1.0);
    } else {
        float wind = (texture(PerlinTexture, WorldPosition.xz/5 + TextureOffset).r);
        FragColor = vec4(mix(LoColor, HiColor, wind), 1.0);
        //FragColor = vec4(wind, wind, wind, 1.0);
    }
}