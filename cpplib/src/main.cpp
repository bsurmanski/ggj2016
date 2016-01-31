#include "draw/sdlWindow.hpp"
#include "draw/scene.hpp"
#include "draw/gl/drawDevice.hpp"
#include "draw/gl/mesh.hpp"
#include "draw/gl/texture.hpp"
#include "draw/gl/mdl.hpp"
#include "draw/tga.hpp"
#include "common/file.hpp"
#include "math/random.hpp"

#include "ring_automaton.hpp"

#include <SDL/SDL.h>
#include <unistd.h>
#include <math.h>

using GL::DrawDevice;

SDLWindow *window;
DrawDevice *drawDevice;
Scene *scene;

Entity *moon1;
Entity *moon2;
Entity *planet;
Entity *ship;

RingAutomaton *life;

bool running = true;

Vec4 player_vector; // normalized position vector
Vec4 player_forward;

enum State {
    TITLE,
    GAME,
};

State state = TITLE;

std::vector<std::vector<bool>> GOL_spaceship = {
   {0, 1, 0, 0, 1},
   {1, 0, 0, 0, 0},
   {1, 0, 0, 0, 1},
   {1, 1, 1, 1, 0},
};

#define GOL_W 80
#define GOL_H 80

#define MOON_ORBIT1 140
#define MOON_ORBIT2 180

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

Mat4 stableBasisFromUpVector(const Vec4 &up) {
    Vec4 fwd = Vec4(1,2,3).orth(up).normalized();
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
    life = new GOLRing(GOL_W, GOL_H);
    life->RandomState();

   const int cx = 0;
   const int cy = 5;
   for (int i = 0; i < GOL_spaceship.size(); ++i) {
     for (int j = 0; j < GOL_spaceship[i].size(); ++j) {
       life->SetCell(j + cx, i + cy, GOL_spaceship[i][j]);
     }
   }

    scene->loadMeshFromFile("planet", "res/planet.msh");
    scene->loadTextureFromFile("planet", "res/planet_dead.tga");

    scene->loadMeshFromFile("moon", "res/moon.msh");
    scene->loadTextureFromFile("moon", "res/moon_tex.tga");
    scene->loadTextureFromFile("moon2", "res/moon2.tga");

    scene->loadMeshFromFile("plant", "res/plant.msh");
    scene->loadTextureFromFile("plant", "res/plant.tga");

    scene->loadMeshFromFile("tree", "res/tree.msh");
    scene->loadTextureFromFile("tree", "res/tree.tga");

    scene->loadMeshFromFile("ship", "res/ship.msh");
    scene->loadTextureFromFile("ship", "res/ship.tga");

    scene->loadTextureFromFile("sky-day", "res/day.tga");
    scene->loadTextureFromFile("sky-night", "res/sky-night.tga");

    scene->loadTextureFromFile("title", "res/title.tga");
    scene->loadTextureFromFile("title-space", "res/title_space.tga");

    moon1 = scene->addEntity("moon");
    moon1->position = Vec4(MOON_ORBIT1, 0, 0);

    moon2 = scene->addEntity("moon", "moon2");
    moon2->position = Vec4(MOON_ORBIT2, 0, 0);

    planet = scene->addEntity("planet");
    planet->scale = Vec4(100, 100, 100);

    ship = scene->addEntity("ship"); 
    ship->position = Vec4(0, 100, 0);

    for(int i = 0; i < 100; i++) {
        Entity *plant = scene->addEntity("plant");
        Vec4 up = randomUnitVector();
        plant->position = up.scaled(100.f);
        plant->rotation = randomBasisFromUpVector(up);
        plant->scale = Vec4(2, 2, 2);
    }

    /*
    for(int i = 0; i < 100; i++) {
        Entity *plant = scene->addEntity("tree");
        Vec4 up = randomUnitVector();
        plant->position = up.scaled(100.f);
        plant->rotation = randomBasisFromUpVector(up);
        plant->scale = Vec4(2, 2, 2);
    }*/

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

    static int spaceDownCount = 0; 
    if(keystate[SDLK_SPACE]) {
        spaceDownCount++;
    } else {
        spaceDownCount = 0;
    }

    if(state == TITLE) {
        if(keystate[SDLK_SPACE]) {
            state = GAME;
        }
    } else if(state == GAME) {
        if(keystate[SDLK_SPACE] && spaceDownCount == 1) {
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
}

void update() {
    if(state == GAME) {
        static int tick = 0;
        tick++;
        if(tick % 100 == 0) {
            life->Poll();
        }

        moon1->position = Vec4(MOON_ORBIT1 * cos((tick+20) / 401.0f), 
                              sin((tick+20) / 97.0f) * 5.0f,
                              MOON_ORBIT1 * sin((tick+20) / 401.0f));

        moon2->position = Vec4(MOON_ORBIT2 * cos(tick/ 301.0f), 
                              sin(tick / 47.0f) * 5.0f,
                              MOON_ORBIT2 * sin(tick / 301.0f));
    }
}

void draw() {
    drawDevice->clear();
    drawDevice->clearFramebuffer();

    if(state == TITLE) {
        static float t = 1;
        t += 0.0005;
        drawDevice->runTitleProgram(scene->getTexture("title"),
                                    scene->getTexture("title-space"),
                                    t);
    } else if (state == GAME) {
        Mat4 vmat;
        Mat4 sky_vmat;
        Vec4 side = player_forward.cross(player_vector).normalized();
        vmat.setXRow(side);
        vmat.setYRow(player_vector);
        vmat.setZRow(player_forward);

        sky_vmat = Mat4().scaled(10, 10, 10) * vmat;

        vmat.translate(Vec4(0, -115, 0));

        Mat4 gaze;
        gaze.rotate(0.6, Vec4(1, 0, 0));
        vmat = gaze * vmat;

        drawDevice->runSkyboxProgram(
                scene->getTexture("sky-day"),
                scene->getTexture("sky-night"),
                sky_vmat, Vec4());

        scene->draw(drawDevice, vmat);

        for(int j = 0; j < GOL_H; j++) {
            for(int i = 0; i < GOL_W; i++) {
                if(life->GetCell(i, j)) {
                    Mat4 mmat;
                    Vec4 p = life->GetCellSphereCoordinate(i, j);
                    mmat = stableBasisFromUpVector(p);
                    mmat.translate(p.scaled(100));
                    drawDevice->runMeshProgram(scene->getMesh("tree"),
                                               scene->getTexture("tree"), 
                                               vmat * mmat);
                    
            //plant->position = up.scaled(100.f);
            //plant->rotation = randomBasisFromUpVector(up);
            //plant->scale = Vec4(2, 2, 2);
                }
            }
        }
    }

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
