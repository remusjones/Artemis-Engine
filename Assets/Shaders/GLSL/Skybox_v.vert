#version 450

layout(push_constant) uniform PushConstants {
    mat4 model;
} inPushConstants;

struct Light{
    vec3 position;
    float lightIntensity;
    vec3 color;
    float ambientStrength;
};

layout(set = 0, binding = 0) uniform SceneBuffer {

    Light mLight;

    mat4 view;
    mat4 proj;

} sceneData;

layout(set = 0, binding = 1) uniform MaterialProperties {
    vec4 color;
    float specularStrength;
    float shininess;
} materialProperties;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBiTangent;
// Vertex Information
layout(location = 0) out vec3 fragUVW;

void main() {
    //TODO: Move the view matrix input to be already stripped of translation components
    gl_Position = sceneData.proj * mat4(mat3(sceneData.view)) * inPushConstants.model * vec4(inPosition, 1.0);
    fragUVW = inPosition;
}