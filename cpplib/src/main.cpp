#include "draw/sdlWindow.hpp"
#include "draw/scene.hpp"
#include "draw/gl/drawDevice.hpp"
#include "draw/gl/mesh.hpp"
#include "draw/gl/texture.hpp"
#include "draw/gl/mdl.hpp"
#include "draw/tga.hpp"
#include "common/file.hpp"

#include <SDL/SDL.h>
#include <unistd.h>

using GL::DrawDevice;

SDLWindow *window;
DrawDevice *drawDevice;
Scene *scene;

Entity *moon;
Entity *planet;

bool running = true;

Vec4 player_vector; // normalized position vector
Vec4 player_forward;

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
    //planet = scene->addEntity("planet");
    scene->addEntity("plant");
    //planet->scale = Vec4(100, 100, 100);

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

    if(keystate[SDLK_LEFT]) {
        Mat4 rot;
        rot.rotate(0.05, player_vector);
        player_forward = rot.mul(player_forward);
        player_forward.normalize();
        player_forward.print();
    }
}

void update() {
    static float tick = 0.0f;
    //tick += 0.01f;
    //planet->qrotation = Vec4::createQuat(tick, Vec4(0, 1, 0));//.qMul(moon->qrotation); 
}

void draw() {
    Mat4 vmat;
    //Vec4 side = player_forward.cross(player_vector).normalized();
    //vmat.setXVector(side);
    //vmat.setYVector(player_vector);
    //vmat.setZVector(player_forward);
    //vmat = vmat.inverse();
 //   Vec4 player_position = player_vector.scaled(110);
    //translation.translate(player_position.scaled(-1));
    //vmat = vmat.translated(player_position);
    //vmat.rotate(-0.5, Vec4(1, 0, 0));
   // vmat.translate(player_position);
   //
    static float f =0.0f;
    f+=0.01;
    vmat.rotate(f, Vec4(1, 1, 0));
   vmat.translate(Vec4(0, 0, -3));

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
