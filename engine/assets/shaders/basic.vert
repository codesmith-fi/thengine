#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec4 outColor;

layout(set = 1, binding = 0) uniform Constants {
    vec2 pos;
    vec2 size;
    vec4 tint;
} pc;

void main() {
    outUV = inUV;
    outColor = inColor * pc.tint;
    
    vec2 worldPos = (inPos * pc.size) + pc.pos;
    
    vec2 screen = vec2(800.0, 600.0);
    
    float clipX = (worldPos.x / screen.x) * 2.0 - 1.0;
    float clipY = 1.0 - (worldPos.y / screen.y) * 2.0;
    
    gl_Position = vec4(clipX, clipY, 0.5, 1.0);
}
