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
uniform mat4 Animation[MAX_BONES];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float signal;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 worldNormal;
out vec2 texCoord;
out vec3 worldPos;
out vec3 tangentLightPos;
out vec3 tangentViewPos;
out vec3 tangentFragPos;

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
    skinningModel  = (weights.x*Animation[jointIndex_x] + weights.y*Animation[jointIndex_y] + weights.z*Animation[jointIndex_z] + weights.w*Animation[jointIndex_w] + weights2.x*Animation[jointIndex2_x] + weights2.y*Animation[jointIndex2_y] + weights2.z*Animation[jointIndex2_z] + weights2.w*Animation[jointIndex2_w]);
}

worldPos = vec3(skinningModel * vec4(aPos, 1.0));
gl_Position = projection * view * vec4(worldPos, 1.0);
worldNormal = mat3(transpose(inverse(skinningModel))) * aNormal;
texCoord = uv_Coord;

//Gosh I have to do all the lighting calculation all over again ugh
vec3 T = normalize(vec3(skinningModel * vec4(aTangent, 0.0)));
vec3 N = normalize(worldNormal);
T = normalize(T - dot(T,N) * N);
vec3 B = cross(N, T);

mat3 TBN = transpose(mat3(T, B, N));
tangentLightPos = TBN * lightPos;
tangentViewPos = TBN * viewPos;
tangentFragPos = TBN * worldPos;

}
