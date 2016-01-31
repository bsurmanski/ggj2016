#include "draw/sdlWindow.hpp"
#include "draw/scene.hpp"
#include "draw/gl/drawDevice.hpp"
#include "draw/gl/mesh.hpp"
#include "draw/gl/texture.hpp"
#include "draw/gl/mdl.hpp"
#include "draw/tga.hpp"
#include "common/file.hpp"
#include "math/random.hpp"

#include <SDL/SDL.h>
#include <unistd.h>
#include <math.h>

using GL::DrawDevice;

SDLWindow *window;
DrawDevice *drawDevice;
Scene *scene;

Entity *moon;
Entity *planet;

bool running = true;

Vec4 player_vector; // normalized position vector
Vec4 player_forward;

Vec4 randomUnitVector() {
    return Vec4((RandomFloat() - 0.5) * M_PI * 4.0f, 
                (RandomFloat() - 0.5) * M_PI * 4.0f, 
                (RandomFloat() - 0.5) * M_PI * 4.0f, 0).normalized();
}

Mat4 randomBasisFromUpVector(const Vec4 &up) {
    Vec4 fwd = randomUnitVector().orth(up).normalized();
    Vec4 side = fwd.cross(up).normalized();
    Mat4 mat;
    mat.setXVector(side);
    mat.setYVector(up.normalized());
    mat.setZVector(fwd);
    return mat;
}

void init() {
    window = new SDLWindow(960, 720, "ggj2016");
    drawDevice = new DrawDevice(960, 720);
    scene = new Scene;
    scene->loadMeshFromFile("planet", "res/planet.msh");
    scene->loadTextureFromFile("planet", "res/planet_dead.tga");

    scene->loadMeshFromFile("moon", "res/planet1.msh");
    scene->loadTextureFromFile("moon", "res/moon_tex.tga");

    scene->loadMeshFromFile("plant", "res/plant.msh");
    scene->loadTextureFromFile("plant", "res/plant.tga");

    //moon = scene->addEntity("moon");
    planet = scene->addEntity("planet");
    planet->scale = Vec4(100, 100, 100);

    for(int i = 0; i < 100; i++) {
        Entity *plant = scene->addEntity("plant");
        Vec4 up = randomUnitVector();
        plant->position = up.scaled(100.f);
        plant->rotation = randomBasisFromUpVector(up);
        plant->scale = Vec4(2, 2, 2);
    }

    player_vector = Vec4(0, 1, 0, 0);
    player_forward = Vec4(0, 0, 1, 0);
}

void input() {
    SDL_PumpEvents();

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            running = false;
        }
    }

    uint8_t *keystate = SDL_GetKeyState(NULL);
    if(keystate[SDLK_SPACE]) {
        running = false;
    }

    if(keystate[SDLK_UP]) {
        player_vector -= player_forward.scaled(0.010);
        player_vector.normalize();
        player_forward = player_forward.orth(player_vector);
        player_forward.normalize();
    }

    if(keystate[SDLK_DOWN]) {
        player_vector += player_forward.scaled(0.007);
        player_vector.normalize();
        player_forward = player_forward.orth(player_vector);
        player_forward.normalize();
    }

    if(keystate[SDLK_LEFT]) {
        Mat4 rot;
        rot.rotate(-0.05, player_vector);
        player_forward = rot.mul(player_forward);
        player_forward.normalize();
    }

    if(keystate[SDLK_RIGHT]) {
        Mat4 rot;
        rot.rotate(0.05, player_vector);
        player_forward = rot.mul(player_forward);
        player_forward.normalize();
    }
}

void update() {
    static float tick = 0.0f;
    //tick += 0.01f;
    //planet->qrotation = Vec4::createQuat(tick, Vec4(0, 1, 0));//.qMul(moon->qrotation); 
}

void draw() {
    Mat4 vmat;
    Vec4 side = player_forward.cross(player_vector).normalized();
    vmat.setXRow(side);
    vmat.setYRow(player_vector);
    vmat.setZRow(player_forward);

    vmat.translate(Vec4(0, -115, 0));

    Mat4 gaze;
    gaze.rotate(0.6, Vec4(1, 0, 0));
    vmat = gaze * vmat;

    drawDevice->clear();
    drawDevice->clearFramebuffer();
    scene->draw(drawDevice, vmat);
    drawDevice->runOutputProgram();
    window->SwapBuffers();
}

int main(int argc, char **argv) {
    init();
    
    while(running) {
        input();
        update();
        draw();
        usleep(10);
    }
    return 0;
}
