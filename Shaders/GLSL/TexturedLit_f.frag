#version 450

layout(location = 0) in vec3 fragAmbient;
layout(location = 1) in vec3 fragDiffuse;
layout(location = 2) in vec3 fragSpecular;
layout(location = 3) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;
layout(set = 0, binding = 3) uniform sampler2D tex1;

void main() {
    // Combine ambient, diffuse, and specular lighting
    vec3 result = fragAmbient + fragDiffuse + fragSpecular;

    // Apply texture
    vec3 texColor = texture(tex1, fragTexCoord).xyz;
    result *= texColor;

    outColor = vec4(result, 1.0f);
}
