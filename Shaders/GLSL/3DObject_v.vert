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
} lightingData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {

    gl_Position = cameraData.viewproj * inPushConstants.model * vec4(inPosition, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(inPushConstants.model)));

    vec3 worldNormal = normalize(normalMatrix * inNormal);

    vec3 lightDir = normalize(lightingData.position - vec3(inPushConstants.model * vec4(inPosition, 1.0)));
    float distance = length(lightingData.position - vec3(inPushConstants.model * vec4(inPosition, 1.0)));

    vec3 ambient = lightingData.ambientStrength * lightingData.color;
    ambient = max(ambient, vec3(0.2));
    float diff = max(dot(worldNormal, lightDir), 0.0);
    vec3 diffuse = diff * lightingData.color / (1.0 + 0.1 * distance + 0.01 * (distance * distance));
    vec3 viewDir = normalize(-vec3(inPushConstants.model * vec4(inPosition, 1.0)));
    vec3 reflectDir = reflect(-lightDir, worldNormal);


    float spec = pow(max(dot(viewDir, reflectDir), 0.0), lightingData.shininess);



    vec3 specular = lightingData.specularStrength * spec * lightingData.color / (1.0 + 0.1 * distance + 0.01 * (distance * distance));

    vec3 result = (ambient + diffuse + specular) * inColor;

    fragColor = result; // Using vec3 for RGB color

}