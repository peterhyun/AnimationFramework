#include "Floor.h"

Floor::Floor() {
    float vertices[] = {
        1000.0, 0.0, 1000.0, 0.0, 1.0, 0.0,
        1000.0, 0.0, -1000.0, 0.0, 1.0, 0.0,
        -1000.0, 0.0, 1000.0, 0.0, 1.0, 0.0,
        -1000.0, 0.0, -1000.0, 0.0, 1.0, 0.0
    };
    _vertices.insert(_vertices.begin(), std::begin(vertices), std::end(vertices));
    unsigned int indices[] = {
        0, 1, 2,
        1, 3, 2
    };
    _indices.insert(_indices.begin(), std::begin(indices), std::end(indices));
    /*
    for (int i = 0; i < _indices.size(); i += 3) {
        std::cout << _indices[i] << _indices[i + 1] << _indices[i + 2] << std::endl;
    }
    */
    setupFloorBufferObjects();
}
void Floor::setupFloorBufferObjects() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * _indices.size(), &_indices[0], GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}
void Floor::draw(Shader* floorShader) {
    glBindVertexArray(VAO);
    floorShader->use();
    //std::cout << "Drawing the floor" << std::endl;
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}