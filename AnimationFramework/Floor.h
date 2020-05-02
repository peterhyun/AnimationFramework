#ifndef FLOOR_H
#define FLOOR_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Shader.h"

class Floor {
private:
    unsigned int VAO, VBO, EBO;
    void setupFloorBufferObjects();

public:
    std::vector<float> _vertices;
    std::vector<unsigned int> _indices;
    void draw(Shader* floorShader);
    Floor();
};

#endif