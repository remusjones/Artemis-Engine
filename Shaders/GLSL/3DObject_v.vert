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
    vec3 color;
    float ambientStrength;
    float specularStrength;
    float shininess;
} LightingData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = cameraData.viewproj * inPushConstants.model * vec4(inPosition, 1.0);
    fragColor = inColor;
}