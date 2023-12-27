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
    float lightIntensity ;
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
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 texCoord;

void main() {

    gl_Position = cameraData.viewproj * inPushConstants.model * vec4(inPosition, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(inPushConstants.model)));

    vec3 worldNormal = normalize(normalMatrix * inNormal);

    vec3 lightDir = normalize(lightingData.position - vec3(inPushConstants.model * vec4(inPosition, 1.0)));
    float distance = length(lightingData.position - vec3(inPushConstants.model * vec4(inPosition, 1.0)));

    vec3 ambient = lightingData.ambientStrength * lightingData.color;
    float diff = max(dot(worldNormal, lightDir), 0.0);
    vec3 diffuse = diff * lightingData.color * lightingData.lightIntensity / (1.0 + 0.1 * distance + 0.01 * (distance
    * distance));
    vec3 viewDir = normalize(-vec3(inPushConstants.model * vec4(inPosition, 1.0)));
    vec3 reflectDir = reflect(-lightDir, worldNormal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialProperties.shininess);
    vec3 specular = materialProperties.specularStrength * spec *
    (lightingData.color * lightingData.lightIntensity) /(1 + 0.1 * distance + 0.01 * (distance * distance));

    // Uses vertex color as influence
    vec3 result = (ambient + diffuse + (specular)) * (inColor * vec3(materialProperties.color));


    fragColor = result;
    texCoord = inTexCoord;

}
