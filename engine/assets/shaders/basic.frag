#version 450

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 outFragColor;

layout(set = 2, binding = 0) uniform sampler2D texSampler;

layout(set = 3, binding = 0) uniform AmbientConstants {
    vec4 ambientColor;
    float ambientIntensity;
} ambient;

void main() {
    vec4 texColor = texture(texSampler, inUV);
    vec4 tintedColor = texColor * inColor;
    
    vec3 litRGB = tintedColor.rgb * (ambient.ambientColor.rgb * ambient.ambientIntensity);
    outFragColor = vec4(litRGB, tintedColor.a);
}
