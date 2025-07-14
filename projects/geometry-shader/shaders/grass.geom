#version 330

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in float height[];

// x = x1*(1-t)^2 + 2*x2*t*(1-t) + x3*t^2
uniform vec2 BezierControlPoint;
uniform float HeightToWidthRatio;
uniform uint8_t Segments;

void main() {
    vec4 xoffset = vec4(height[0]*.5*HeightToWidthRatio, 0.0, 0.0, 0.0);
    vec4 yoffset = vec4(0.0, height[0], 0.0, 0.0);
    
    gl_Position = gl_in[0].gl_Position - xoffset;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + xoffset;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position - xoffset + yoffset;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + xoffset + yoffset;
    EmitVertex();
    EndPrimitive();
}