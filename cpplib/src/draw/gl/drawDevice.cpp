#include "drawDevice.hpp"

#include "common/file.hpp"
#include "draw/gl/mdl.hpp"

using GL::DrawDevice;
using GL::Framebuffer;
using GL::Texture;
using GL::RGBATexture;
using GL::DepthTexture;
using GL::Mesh;
using GL::Program;

DrawDevice *DrawDevice::instance = NULL;

DrawDevice::DrawDevice(int _w, int _h) : w(_w), h(_h) {
    glewInit();

    if(!instance) instance = this;
   
    mainBuffer = new Framebuffer();
    colorTexture = new RGBATexture(_w, _h);
    normalTexture = new RGBATexture(_w, _h);
    depthTexture = new DepthTexture(_w, _h);

    mainBuffer->addTarget(colorTexture);
    mainBuffer->addTarget(normalTexture);
    mainBuffer->addTarget(depthTexture);

    File *simplevs = new File("res/glsl/simple.vs");
    File *simplefs = new File("res/glsl/simple.fs");
    simpleProgram = new StdProgram(simplevs, simplefs);
    delete simplevs;
    delete simplefs;

    File *meshvs = new File("res/glsl/mesh.vs");
    File *meshfs = new File("res/glsl/mesh.fs");
    meshProgram = new StdProgram(meshvs, meshfs);
    delete meshvs;
    delete meshfs;

    File *quadFile = new File("res/unit_quad.mdl");
    File *cubeFile = new File("res/unit_cube.mdl");
    quad = loadMdl(quadFile);
    cube = loadMdl(cubeFile);
    delete quadFile;
    delete cubeFile;

    glFrontFace(GL_CW);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    //glEnable(GL_CULL_TEST);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_SCISSOR_TEST);
}

void DrawDevice::bindStandardAttributes(Program *prog) {
    GLint pos = glGetAttribLocation(prog->program, "position");
    GLint norm = glGetAttribLocation(prog->program, "normal");
    GLint uv = glGetAttribLocation(prog->program, "uv");
    GLint color = glGetAttribLocation(prog->program, "color");

    if(pos >= 0) {
        glEnableVertexAttribArray(pos);
        glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 32, NULL);
    }

    if(norm >= 0) {
        glEnableVertexAttribArray(norm);
        glVertexAttribPointer(norm, 3, GL_SHORT, GL_TRUE, 32, (void*) 12);
    }

    if(uv >= 0) {
        glEnableVertexAttribArray(uv);
        glVertexAttribPointer(uv, 2, GL_UNSIGNED_SHORT, GL_TRUE, 32, (void*) 18);
    }

    if(color >= 0) {
        glEnableVertexAttribArray(color);
        glVertexAttribPointer(color, 3, GL_UNSIGNED_BYTE, GL_TRUE, 32, (void*) 22);
    }
}

DrawDevice *DrawDevice::getInstance() {
    return instance;
}

void DrawDevice::clear() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void DrawDevice::clearFramebuffer() {
    mainBuffer->bind();
    glViewport(0, 0, w, h); // framebuffer w/h?
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void DrawDevice::runMeshProgram(Mesh *m, Texture *t, const Mat4 &mv) {
}

void DrawDevice::runSimpleProgram(Mesh *m, Texture *t, const Mat4 &mv) {
}

#include <stdio.h>
void DrawDevice::test() {
    //mainBuffer->bind();
    //

    static float r = 0.0f;
    r += 0.05;
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, w, h);
    simpleProgram->bind();
    cube->bind();
    bindStandardAttributes(simpleProgram);
    Mat4 m;
    Mat4 f = Mat4::createFrustumMatrix(-1, 1, -1, 1, 1, 100);
    m.rotate(r, Vec4(0, 1, 1));
    m = m.translated(Vec4(0, 0, -5));
    m = f * m;
    glUniformMatrix4fv(glGetUniformLocation(simpleProgram->program, "mvp"),
                        1, GL_TRUE, m.ptr());
    cube->draw();
}
