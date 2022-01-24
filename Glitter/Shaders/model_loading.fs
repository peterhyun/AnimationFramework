#version 330 core
out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;

in vec2 texCoord;
in vec3 tangentLightPos;
in vec3 tangentViewPos;
in vec3 tangentFragPos;
in vec3 worldNormal;

void main()
{

vec3 normal = texture(normalTexture, texCoord).rgb;
normal = normalize(normal * 2.0 - 1.0);

//No normal mapping
//vec3 normal = normalize(worldNormal);

vec3 diffuseColor = texture(diffuseTexture, texCoord).rgb;

vec3 ambient = 0.3 * diffuseColor;

vec3 lightDir = normalize(tangentLightPos - tangentFragPos);
float diff = max(dot(normal, lightDir), 0.0);
vec3 diffuse = diff * diffuseColor;

vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
vec3 reflectDir = reflect(-lightDir, normal);
vec3 halfwayDir = normalize(lightDir + viewDir);
float spec = pow(max(dot(normal, halfwayDir), 0.0), 8.0);

vec3 specularColor = texture(specularTexture, texCoord).rgb;

//Add 0.5 cause her skin looks like copper
vec3 specular = 0.5 * spec * specularColor;

//FragColor = vec4(ambient + ((1.0-shadow) * (diffuse + specular)), 1.0);
FragColor = vec4(ambient + diffuse + specular, 1.0);
//FragColor = vec4(1.0);// + diffuse + specular, 1.0);

/*
vec3 ambient = vec3(0.3) * vec3(texture(diffuseTexture, texCoord));

vec3 norm = normalize(worldNormal);
vec3 lightDir = normalize(lightDirection);
float diff = max(dot(norm, lightDir), 0.0);
vec3 diffuse = vec3(0.8) * diff * vec3(texture(diffuseTexture, texCoord));

vec3 viewDir = normalize(viewPos-worldPos);
vec3 reflectDir = reflect(-lightDir, norm);
float spec = pow(max(dot(viewDir, reflectDir), 0.0), 10.0);
vec3 specular = vec3(0.5) * spec * vec3(texture(specularTexture, texCoord));

//float shadow = ShadowCalculation(fragPosLightSpace);

//FragColor = vec4(ambient + ((1.0-shadow) * (diffuse + specular)), 1.0);
FragColor = vec4(ambient + diffuse + specular, 1.0);
//FragColor = vec4(1.0);// + diffuse + specular, 1.0);
*/
}
