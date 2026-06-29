#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec4 outColor;
layout(location = 2) out vec2 outWorldPos;

layout(set = 1, binding = 0) uniform Constants {
    mat4 transform;
    vec4 tint;
} pc;

void main() {
    outUV = inUV;
    outColor = inColor * pc.tint;
    outWorldPos = inPos;
    
    gl_Position = pc.transform * vec4(inPos, 0.0, 1.0);
}
