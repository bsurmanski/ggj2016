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
    colorTexture = new RGBATexture(_w/2, _h/2);
    normalTexture = new RGBATexture(_w/2, _h/2);
    depthTexture = new DepthTexture(_w/2, _h/2);

    mainBuffer->addTarget(colorTexture);
    mainBuffer->addTarget(normalTexture);
    mainBuffer->addTarget(depthTexture);

    File *simplevs = new File("res/glsl/simple.vs");
    File *simplefs = new File("res/glsl/simple.fs");
    simpleProgram = new StdProgram(simplevs, simplefs);
    delete simplevs;
    delete simplefs;

    File *deferredvs = new File("res/glsl/deferred.vs");
    File *deferredfs = new File("res/glsl/deferred.fs");
    deferredProgram = new StdProgram(deferredvs, deferredfs);
    delete deferredvs;
    delete deferredfs;

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
    glDisable(GL_SCISSOR_TEST);
    //glEnable(GL_CULL_TEST);
    glEnable(GL_DEPTH_TEST);
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
    glViewport(0, 0, w/2, h/2); // framebuffer w/h?
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void DrawDevice::runMeshProgram(Mesh *mesh, Texture *tex, const Mat4 &mv) {
    glEnable(GL_DEPTH_TEST);
    mainBuffer->bind();

    tex->bind();
    glViewport(0, 0, w/2, h/2);
    simpleProgram->bind();
    mesh->bind();
    bindStandardAttributes(simpleProgram);
    Mat4 f = Mat4::createFrustumMatrix(-1, 1, -1, 1, 2, 100);
    f = f * mv;
    glUniformMatrix4fv(glGetUniformLocation(simpleProgram->program, "mvp"),
                        1, GL_TRUE, f.ptr());
    mesh->draw();
}

void DrawDevice::runOutputProgram() {
    glDisable(GL_DEPTH_TEST);
    colorTexture->bind(0);
    normalTexture->bind(1);
    depthTexture->bind(2);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, w, h);

    static float f = 0.0f;
    f += 0.004;
    deferredProgram->bind();
    quad->bind();
    bindStandardAttributes(deferredProgram);
    glUniform1i(glGetUniformLocation(deferredProgram->program, "t_color"), 0);
    glUniform1i(glGetUniformLocation(deferredProgram->program, "t_normal"), 1);
    glUniform1i(glGetUniformLocation(deferredProgram->program, "t_depth"), 2);
    glUniform1f(glGetUniformLocation(deferredProgram->program, "tick"), f);
    quad->draw();
}

void DrawDevice::runSimpleProgram(Mesh *m, Texture *t, const Mat4 &mv) {
}
