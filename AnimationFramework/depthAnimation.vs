#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 uv_Coord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec4 jointIndex;
layout (location = 5) in vec4 jointIndex2;
layout (location = 6) in vec4 weights;
layout (location = 7) in vec4 weights2;

const int MAX_BONES = 100;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform mat4 Animation[MAX_BONES];
uniform float signal;

void main()
{
int jointIndex_x = int(jointIndex.x);
int jointIndex_y = int(jointIndex.y);
int jointIndex_z = int(jointIndex.z);
int jointIndex_w = int(jointIndex.w);
int jointIndex2_x = int(jointIndex2.x);
int jointIndex2_y = int(jointIndex2.y);
int jointIndex2_z = int(jointIndex2.z);
int jointIndex2_w = int(jointIndex2.w);

mat4 skinningModel;

if(signal==0.0){
    skinningModel = model;
}
else{
    skinningModel  = ((weights.x*Animation[jointIndex_x] + weights.y*Animation[jointIndex_y] + weights.z*Animation[jointIndex_z] + weights.w*Animation[jointIndex_w] + weights2.x*Animation[jointIndex2_x] + weights2.y*Animation[jointIndex2_y] + weights2.z*Animation[jointIndex2_z] + weights2.w*Animation[jointIndex2_w]));
}
    gl_Position = lightSpaceMatrix * skinningModel * vec4(aPos, 1.0);
}
