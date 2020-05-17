#include "Axis.h"

Axis::Axis() {
    float vertices[] = {
        0.0, 0.0, 0.0, 1.0,
        5.0, 0.0, 0.0, 1.0,

        0.0, 0.0, 0.0, 2.0,
        0.0, 5.0, 0.0, 2.0,

        0.0, 0.0, 0.0, 3.0,
        0.0, 0.0, 5.0, 3.0
    };
    _vertices.insert(_vertices.begin(), std::begin(vertices), std::end(vertices));
    /*
    for (int i = 0; i < _indices.size(); i += 3) {
        std::cout << _indices[i] << _indices[i + 1] << _indices[i + 2] << std::endl;
    }
    */
    setupAxisBufferObjects();
}

void Axis::setupAxisBufferObjects() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}
void Axis::draw(Shader* axisShader) {
    glBindVertexArray(VAO);
    axisShader->use();
    //std::cout << "Drawing the axis" << std::endl;
    glDrawArrays(GL_LINES, 0, 6);
    //glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}