#ifndef AXIS_H
#define AXIS_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Shader.h"

class Axis {
private:
    unsigned int VAO, VBO;
    void setupAxisBufferObjects();

public:
    std::vector<float> _vertices;
    void draw(Shader* axisShader);
    Axis();
};

#endif