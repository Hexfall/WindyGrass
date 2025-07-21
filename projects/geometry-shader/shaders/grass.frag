#version 330

in vec3 WorldPosition;
in vec3 Normal;
in float StalkDist;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D GrassTexture;

// Blinn-phong properties
uniform float AmbientReflection = 1.0;
uniform float DiffuseReflection = 1.0;
uniform float SpecularReflection = 1.0;
uniform float SpecularExponent = 10.0;

// Light information
uniform vec3 AmbientColor = vec3(0.25);
uniform vec3 LightColor = vec3(1.0);
uniform vec3 LightVector = normalize(vec3(0.75, 1.0, 0.0));
uniform vec3 CameraPosition;

vec3 GetAmbientReflection(vec3 objectColor) {
    return AmbientColor * AmbientReflection * objectColor;
}

vec3 GetDiffuseReflection(vec3 objectColor, vec3 lightVector, vec3 normalVector) {
    return LightColor * DiffuseReflection * objectColor * max(dot(lightVector, normalVector), 0.0f);
}

vec3 GetSpecularReflection(vec3 lightVector, vec3 viewVector, vec3 normalVector) {
    vec3 halfVector = normalize(lightVector + viewVector);
    return LightColor * SpecularReflection * pow(max(dot(halfVector, normalVector), 0.0f), SpecularExponent);
}

vec3 GetBlinnPhongReflection(vec3 objectColor, vec3 lightVector, vec3 viewVector, vec3 normalVector) {
    return GetAmbientReflection(objectColor)
    + GetDiffuseReflection(objectColor, lightVector, normalVector)
    + GetSpecularReflection(lightVector, viewVector, normalVector);
}

void main() {
    vec3 viewVector = normalize(CameraPosition - WorldPosition);
    vec3 normalVector = normalize(Normal);
    float factor = pow(StalkDist*2, 2.0);
    vec4 color = texture(GrassTexture, TexCoord);
    FragColor = vec4(GetBlinnPhongReflection(color.rgb, LightVector, viewVector, normalVector), 1.0f);
}