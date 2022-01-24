#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 uv_Coord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in uvec4 jointIndex;
layout (location = 5) in uvec4 jointIndex2;
layout (location = 6) in vec4 weights;
layout (location = 7) in vec4 weights2;

const int MAX_BONES = 100;
uniform mat4 Animation[MAX_BONES];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float signal;

out vec3 worldNormal;
out vec3 worldPos;

void main()
{
    uint jointIndex_x = jointIndex.x;
    uint jointIndex_y = jointIndex.y;
    uint jointIndex_z = jointIndex.z;
    uint jointIndex_w = jointIndex.w;
    uint jointIndex2_x = jointIndex2.x;
    uint jointIndex2_y = jointIndex2.y;
    uint jointIndex2_z = jointIndex2.z;
    uint jointIndex2_w = jointIndex2.w;
    
    mat4 skinningModel;
    
    if(signal==0.0){
        skinningModel = model;
    }
    else{
        skinningModel  = (weights.x*Animation[jointIndex_x] + weights.y*Animation[jointIndex_y] + weights.z*Animation[jointIndex_z] + weights.w*Animation[jointIndex_w] + weights2.x*Animation[jointIndex2_x] + weights2.y*Animation[jointIndex2_y] + weights2.z*Animation[jointIndex2_z] + weights2.w*Animation[jointIndex2_w]);
    }
    
    vec3 worldNormal_temp = mat3(transpose(inverse(skinningModel))) * normalize(aNormal);
    worldNormal = normalize(worldNormal_temp);
    gl_Position = projection * view * skinningModel * vec4(aPos + worldNormal*0.30, 1.0);
    worldPos = vec3(skinningModel * vec4(aPos, 1.0));
}
