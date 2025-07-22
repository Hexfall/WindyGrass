#version 330

layout (points) in;
layout (triangle_strip, max_vertices = 33) out;

in float height[];
in float angle[];
in float timeOffset[];

out vec3 WorldPosition;
out vec3 Normal;
out vec2 TexCoord;

uniform float HeightToWidthRatio;
uniform float HeightToLengthRatio;
uniform uint Segments;

uniform vec2 BezStalk = vec2(0, 2.34);
uniform vec2 BezPull = vec2(1.48, 6.51);
uniform vec2 BezEnd = vec2(3.21, 6.57);

uniform float Time;
uniform vec2 SwayAmount = vec2(1, -1);
uniform float SwaySpeed = 1.0;

uniform float NormalRounding = 3.1415/12;

uniform sampler2D WindMap;
uniform vec2 WindOffset;
uniform vec2 WindSway;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;

mat4 rotateAroundAxis(vec3 axis, float angle) {
    float c = cos(angle);
    float s = sin(angle);
    float t = 1.0 - c;

    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    return mat4(
    t*x*x + c,   t*x*y - s*z, t*x*z + s*y, 0.0,
    t*x*y + s*z, t*y*y + c,   t*y*z - s*x, 0.0,
    t*x*z - s*y, t*y*z + s*x, t*z*z + c, 0.0,
    0.0, 0.0, 0.0, 1.0
    );
}

mat4 inverseRotation(mat4 rotation) {
    return transpose(rotation);
}

// Quadratic Bezier equation, assuming v0 = 0. Result is normalized.
float bezierPoint(float step, float v1, float v2, float v3, float clamp) {
    float p1 = 3.0*pow(1.0-step, 2.0)*step*v1;
    float p2 = 3.0*(1.0-step)*pow(step, 2.0)*v2;
    float p3 = pow(step, 3.0)*v3;
    return (p1 + p2 + p3) / clamp;
}

// Derivitive of quadratic Bezier equation for finding curve tangent. Result is normalized.
float bezierDerivativePoint(float step, float v1, float v2, float v3) {
    float p1 = (v1/v3)*(3.0*pow(1.0-step, 2.0) - 6.0*step*(1.0-step));
    float p2 = (v2/v3)*(6.0*step*(1.0-step) - 3.0*pow(step, 2.0));
    float p3 = 3.0*pow(step, 2.0); // v3/v3 is 1, and is therefore omitted.
    return p1 + p2 + p3;
}

// X coord
float getXCoord(float step) {
    return (0.000386183*pow(step*20, 3.0f) - 0.014326*pow(step*20, 2.0f) + 0.0540403*step*20 + 1.56213)/1.56213;
}

// Calculates the coordinates for tapered edge of a blade of grass using a polynomial.
float getYCoord(float step) {
    float sway = sin(timeOffset[0]+Time*SwaySpeed)*SwayAmount.y;
    float wind = (texture(WindMap, WorldPosition.xz/5 + WindOffset).r) * WindSway.y;
    return bezierPoint(step, BezStalk.y, BezPull.y, BezEnd.y + sway + wind, BezEnd.y);
}

// Calculates the bend of the grass with wind considered using a bezier curve.
float getZCoord(float step) {
    float sway = sin(timeOffset[0]+Time*SwaySpeed)*SwayAmount.x;
    float wind = (texture(WindMap, gl_in[0].gl_Position.xz/10).r + .5) * WindSway.x;
    return bezierPoint(step, BezStalk.x, BezPull.x, BezEnd.x + sway + wind, BezEnd.x);
}

vec3 getBezierTangent(float step) {
    float windy = (texture(WindMap, gl_in[0].gl_Position.xz/10).r + .5) * WindSway.y;
    float swayy = sin(timeOffset[0]+Time*SwaySpeed)*SwayAmount.y;
    float y = bezierDerivativePoint(step, BezStalk.y, BezPull.y, BezEnd.y + swayy + windy) * height[0];

    float windz = (texture(WindMap, gl_in[0].gl_Position.xz/10).r + .5) * WindSway.x;
    float swayz = sin(timeOffset[0]+Time*SwaySpeed)*SwayAmount.x;
    float z = bezierDerivativePoint(step, BezStalk.x, BezPull.x, BezEnd.x + swayz + windz) * height[0] * HeightToLengthRatio;
    
    return vec3(0.0, y, z);
}

vec3 getCurveNormal(vec3 tangent) {
    return cross(vec3(-1.0, 0.0, 0.0), tangent);
}

void main() {
    mat4 rotationMatrix = mat4(
            cos(angle[0]), 0.0, sin(angle[0]), 0.0,
            0.0, 1.0, 0.0, 0.0,
            -sin(angle[0]), 0.0, cos(angle[0]), 0.0,
            0.0, 0.0, 0.0, 1.0
    );
    
    for (uint i = Segments; i > 0u; i--) {
        float step = (Segments - i)/float(Segments);
        vec4 offset = vec4(
            getXCoord(step)*height[0]*HeightToWidthRatio, 
            getYCoord(step)*height[0], 
            getZCoord(step)*height[0]*HeightToLengthRatio, 
            0);
        vec3 tangent = normalize(getBezierTangent(step));
        mat4 normalRotation = rotateAroundAxis(tangent, NormalRounding);
        vec3 curveNormal = getCurveNormal(tangent);

        // Left vertex
        TexCoord = vec2(0.0, step);
        Normal = normalize((WorldMatrix * rotationMatrix * inverseRotation(normalRotation) * vec4(curveNormal, 0.0)).xyz);
        vec4 localPosition = rotationMatrix * vec4(-offset.x, offset.yzw);
        WorldPosition = (WorldMatrix * (gl_in[0].gl_Position + localPosition)).xyz;
        gl_Position = ViewProjMatrix * vec4(WorldPosition, 1.0);
        EmitVertex();
        
        // Right vertex
        TexCoord = vec2(1.0, step);
        Normal = normalize((WorldMatrix * rotationMatrix * normalRotation * vec4(curveNormal, 0.0)).xyz);
        localPosition = rotationMatrix * offset;
        WorldPosition = (WorldMatrix * (gl_in[0].gl_Position + localPosition)).xyz;
        gl_Position = ViewProjMatrix * vec4(WorldPosition, 1.0);
        EmitVertex();
    }

    TexCoord = vec2(0.5, 1.0);
    Normal = (WorldMatrix * rotationMatrix * vec4(getCurveNormal(normalize(getBezierTangent(1.0))), 0.0)).xyz;
    vec4 localPosition = rotationMatrix * vec4(0, getYCoord(1)*height[0], getZCoord(1)*height[0]*HeightToLengthRatio, 0.0);
    WorldPosition = (WorldMatrix * (gl_in[0].gl_Position + localPosition)).xyz;
    gl_Position = ViewProjMatrix * vec4(WorldPosition, 1.0);
    EmitVertex();
    EndPrimitive();
}