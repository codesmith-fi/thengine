#version 450

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 outWorldPos;

layout(location = 0) out vec4 outFragColor;

layout(set = 2, binding = 0) uniform sampler2D texSampler;

struct PointLight {
    vec2 position;
    float radius;
    float intensity;
    vec4 color;
};

layout(set = 3, binding = 0) uniform LightingConstants {
    vec4 ambientColor;
    float ambientIntensity;
    int activeLightCount;
    float padding1[2];
    
    PointLight lights[10];
} lighting;

void main() {
    vec4 texColor = texture(texSampler, inUV);
    vec4 tintedColor = texColor * inColor;
    
    vec3 finalLight = lighting.ambientColor.rgb * lighting.ambientIntensity;
    
    for (int i = 0; i < lighting.activeLightCount; i++) {
        vec2 diff = lighting.lights[i].position - outWorldPos;
        float dist = length(diff);
        if (dist < lighting.lights[i].radius) {
            float atten = clamp(1.0 - (dist / lighting.lights[i].radius), 0.0, 1.0);
            finalLight += lighting.lights[i].color.rgb * lighting.lights[i].intensity * atten;
        }
    }
    
    outFragColor = vec4(tintedColor.rgb * finalLight, tintedColor.a);
}
