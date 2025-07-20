#version 330

layout (points) in;
layout (triangle_strip, max_vertices = 33) out;

in float height[];
in float angle[];
in float timeOffset[];

out vec4 Color;

// x = x1*(1-t)^2 + 2*x2*t*(1-t) + x3*t^2
uniform vec2 BezStart = vec2(0, 0);
uniform vec2 BezStalk = vec2(0, 2.34);
uniform vec2 BezPull = vec2(1.48, 6.51);
uniform vec2 BezEnd = vec2(3.21, 6.57);
uniform vec2 BezScale = vec2(3.21, 6.57);
uniform float HeightToWidthRatio;
uniform float HeightToLengthRatio;
uniform uint Segments;

uniform float Time;
uniform vec2 SwayAmount = vec2(1, -1);
uniform float SwaySpeed = 1.0;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;

// X coord
float getXCoord(float step) {
    return (0.000386183*pow(step*20, 3.0f) - 0.014326*pow(step*20, 2.0f) + 0.0540403*step*20 + 1.56213)/1.56213;
}

// Calculates the coordinates for tapered edge of a blade of grass using a polynomial.
float getYCoord(float step) {
    return (3.0*pow(1.0-step, 2.0)*step*BezStalk.y + 3.0*(1.0-step)*pow(step, 2.0)*BezPull.y + pow(step, 3.0)*(BezEnd.y+sin(timeOffset[0]+Time*SwaySpeed)*SwayAmount.y))/BezScale.y;
}

// Calculates the bend of the grass with wind considered using a bezier curve.
float getZCoord(float step) {
    return (3.0*pow(1.0-step, 2.0)*step*BezStalk.x + 3.0*(1.0-step)*pow(step, 2.0)*BezPull.x + pow(step, 3.0)*(BezEnd.x+sin(timeOffset[0]+Time*SwaySpeed)*SwayAmount.x))/BezScale.x;
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
        
        // Left vertex
        Color = vec4(getYCoord(step), 1, 0, 0);
        gl_Position = ViewProjMatrix * WorldMatrix * rotationMatrix * (gl_in[0].gl_Position + vec4(-offset.x, offset.yzw));
        EmitVertex();
        
        // Right vertex
        Color = vec4(offset.xy, 0, 0);
        gl_Position = ViewProjMatrix * WorldMatrix * rotationMatrix * (gl_in[0].gl_Position + offset);
        EmitVertex();
    }
    Color = vec4(getYCoord(1), 1, 0, 0);
    gl_Position = ViewProjMatrix * WorldMatrix * rotationMatrix * (gl_in[0].gl_Position + vec4(0, getYCoord(1)*height[0], getZCoord(1)*height[0]*HeightToLengthRatio, 0));//getZCoord(1), 0));
    EmitVertex();
    EndPrimitive();
}