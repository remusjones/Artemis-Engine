#version 450


layout(push_constant)  uniform PushConstants {
    mat4 model;
} inPushConstants;

layout(set = 0, binding = 0) uniform CameraBuffer{
    mat4 view;
    mat4 proj;
    mat4 viewproj;
} cameraData;

layout(set = 0, binding = 1) uniform LightingBuffer{
    vec3 position;
    float Strength;
    vec3 color;
    float ambientStrength;
} lightingData;

layout(set = 0, binding = 2) uniform MaterialProperties{
    vec4 color;
    float specularStrength;
    float shininess;
} materialProperties;

// Vertex Information
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec3 inUV;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 uv;

void main() {

    gl_Position = cameraData.viewproj * inPushConstants.model * vec4(inPosition, 1.0);

    vec3 result = vec3(materialProperties.color);
    fragColor = result;

}