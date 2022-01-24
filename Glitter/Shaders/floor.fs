#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec4 fragPosLightSpace;
in vec3 normal;

uniform vec3 viewPos;
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace){
    //convert to normalized device coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    //convert it to depth map range
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    float bias = 0.0005;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int x = -3; x <= 3; ++x)
    {
        for(int y = -3; y <= 3; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += ((currentDepth - bias) > pcfDepth  ? 1.0 : 0.0);        
        }    
    }
    shadow /= 25.0; 
    
    /*
    float pcfDepth = texture(shadowMap, projCoords.xy).r; 
    shadow = ((currentDepth - bias) > pcfDepth ? 1.0 : 0.0);
    */
    
    // keep the shadow when region is out of light's frustum.
    if(projCoords.z > 1.0 || projCoords.x > 1.0 || projCoords.y > 1.0)
        shadow = 0.0;

    return shadow;
}

void main()
{
    vec2 uv;
    uv.x = (FragPos.x+1000.0)/2000.0;
    uv.y = (FragPos.z+1000.0)/2000.0;
    float size = 1.0/32.0;   // size of the tile
    float edge = size/32.0; // size of the edge
    uv = (mod(uv, size) - mod(uv - edge, size) - edge) * 1.0/size;
    float distance = length(viewPos-FragPos);
    float fogLevel = 1.0/exp(0.0015*distance);
    float tilePattern = 0.9 - length(uv) * 0.5; 
    
    FragColor = (1.0-ShadowCalculation(fragPosLightSpace))  * vec4(mix(vec3(0.7, 0.7, 0.7), vec3(tilePattern, tilePattern, tilePattern), fogLevel), 1.0);
}
