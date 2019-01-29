#include "mesh.hpp"

#include <stdlib.h>

using GL::Mesh;
using GL::Vertex;
using GL::Face;

Mesh::Mesh() {
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
    glGenVertexArrays(1, &vao);
    nelems = 0;
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);
}

void Mesh::uploadVertexData(Vertex *verts, int nverts) {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * nverts,
                 verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, NULL);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_SHORT, GL_TRUE, 32, (void*) 12);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_UNSIGNED_SHORT, GL_TRUE, 32, (void*) 18);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_UNSIGNED_BYTE, GL_TRUE, 32, (void*) 22);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

void Mesh::uploadFaceData(Face *faces, int nfaces) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Face) * nfaces,
                 faces, GL_STATIC_DRAW);
    nelems = nfaces * 3;
}

void Mesh::bind() {
    glBindVertexArray(vao);
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

#include <stdio.h>
void Mesh::draw() {
     glDrawElements(GL_TRIANGLES, nelems, GL_UNSIGNED_SHORT, NULL);
}
