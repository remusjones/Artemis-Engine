#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec4 outColor;
layout(set = 0, binding = 3) uniform sampler2D tex1;

void main() {
    vec3 color = texture(tex1,texCoord).xyz;
    outColor = vec4(color,1.0f);
}