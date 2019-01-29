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

#include <SDL2/SDL.h>
#ifdef __APPLE__
#include <SDL2_mixer/SDL_mixer.h>
#else
#include <SDL2/SDL_mixer.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

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
Entity *earth;
Entity *death = NULL;

RingAutomaton *life;

Mix_Chunk *boom;
Mix_Chunk *music;

bool running = true;

Vec4 player_vector; // normalized position vector
Vec4 player_forward;

enum State {
    TITLE,
    GAME,
    END,
};

State state = TITLE;

bool everyOneDies = false;
float fade = 0.0f;

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
    Vec4 fwd = Vec4(0,1,0).orth(up).normalized();
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
    //life = new PacmanMap;
    life = new GOLRing(80, 80);
    life->RandomState();

    /*
   const int cx = 0;
   const int cy = 5;
   for (int i = 0; i < GOL_spaceship.size(); ++i) {
     for (int j = 0; j < GOL_spaceship[i].size(); ++j) {
       life->SetCell(j + cx, i + cy, GOL_spaceship[i][j]);
     }
   }
   */

    scene->loadMeshFromFile("planet", "res/planet.msh");
    scene->loadTextureFromFile("planet", "res/planet_dead.tga");

    scene->loadMeshFromFile("moon", "res/moon.msh");
    scene->loadTextureFromFile("moon", "res/moon_tex.tga");
    scene->loadTextureFromFile("moon2", "res/moon2.tga");

    scene->loadMeshFromFile("plant", "res/plant.msh");
    scene->loadTextureFromFile("plant", "res/plant.tga");

    scene->loadMeshFromFile("tree", "res/tree.msh");
    scene->loadTextureFromFile("tree", "res/tree.tga");

    scene->loadMeshFromFile("ghost", "res/ghost.msh");
    scene->loadTextureFromFile("ghost", "res/ghost.tga");

    scene->loadMeshFromFile("grave", "res/grave.msh");
    scene->loadTextureFromFile("grave", "res/grave.tga");

    scene->loadMeshFromFile("ship", "res/ship.msh");
    scene->loadTextureFromFile("ship", "res/ship.tga");

    scene->loadMeshFromFile("death", "res/death.msh");
    scene->loadTextureFromFile("death", "res/death.tga");

    scene->loadMeshFromFile("sh", "res/stonehenge.msh");
    scene->loadTextureFromFile("sh", "res/stonehenge.tga");

    scene->loadMeshFromFile("earth", "res/earth.msh");
    scene->loadTextureFromFile("earth", "res/earth.tga");

    scene->loadMeshFromFile("explode", "res/death.msh");
    scene->loadTextureFromFile("explode", "res/explode.tga");

    scene->loadTextureFromFile("sky-day", "res/day.tga");
    scene->loadTextureFromFile("sky-night", "res/sky-night.tga");

    scene->loadTextureFromFile("title", "res/title.tga");
    scene->loadTextureFromFile("title-space", "res/title_space.tga");

    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 2048);
    boom = Mix_LoadWAV("res/death.wav");

    Mix_PlayChannelTimed(-1, Mix_LoadWAV("res/start.wav"), 0, -1);
    music = Mix_LoadWAV("res/music.wav");

    moon1 = scene->addEntity("moon");
    moon1->position = Vec4(MOON_ORBIT1, 0, 0);

    moon2 = scene->addEntity("moon", "moon2");
    moon2->position = Vec4(MOON_ORBIT2, 0, 0);

    planet = scene->addEntity("planet");
    planet->scale = Vec4(100, 100, 100);

    ship = scene->addEntity("ship");
    ship->position = Vec4(0, -100, 0);
    ship->rotation = Mat4();
    ship->rotation.rotate(M_PI, Vec4(1, 0, 0));

    // stonehenge
    Entity *sh = scene->addEntity("sh");
    sh->position = Vec4(0, 97, 0);

    earth = scene->addEntity("earth");
    earth->position = Vec4(0, -400, 0);
    earth->rotation.rotate(M_PI/2, Vec4(1, 0, 0));

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

    const uint8_t *keystate = SDL_GetKeyboardState(NULL);

    static int spaceDownCount = 0;
    if(keystate[SDL_SCANCODE_SPACE]) {
        spaceDownCount++;
    } else {
        spaceDownCount = 0;
    }

    if(state == TITLE) {
        if(keystate[SDL_SCANCODE_SPACE]) {
            Mix_PlayChannelTimed(-1, Mix_LoadWAV("res/startGame.wav"), 0, -1);
            Mix_PlayChannelTimed(-1, music, -1, -1);
            state = GAME;
        }
    } else if(state == GAME) {
        if(keystate[SDL_SCANCODE_SPACE] && spaceDownCount == 1) {
            running = false;
        }

        if(keystate[SDL_SCANCODE_UP]) {
            player_vector -= player_forward.scaled(0.010);
            player_vector.normalize();
            player_forward = player_forward.orth(player_vector);
            player_forward.normalize();
        }

        if(keystate[SDL_SCANCODE_DOWN]) {
            player_vector += player_forward.scaled(0.007);
            player_vector.normalize();
            player_forward = player_forward.orth(player_vector);
            player_forward.normalize();
        }

        if(keystate[SDL_SCANCODE_LEFT]) {
            Mat4 rot;
            rot.rotate(-0.05, player_vector);
            player_forward = rot.mul(player_forward);
            player_forward.normalize();
        }

        if(keystate[SDL_SCANCODE_RIGHT]) {
            Mat4 rot;
            rot.rotate(0.05, player_vector);
            player_forward = rot.mul(player_forward);
            player_forward.normalize();
        }
    } else if(state == END) {
        if(keystate[SDL_SCANCODE_SPACE] && spaceDownCount == 1) {
            running = false;
        }
    }
}

void update() {
    if(player_vector.distance(Vec4(0, -1, 0)) < 0.3) {
        state = END;
    }

    if(state ==END) {
        if(everyOneDies) {
            if(death == NULL) {
                death = scene->addEntity("death");
                death->position = Vec4(-100, -100, -100);
                death->rotation = Mat4();
                death->rotation.rotate(M_PI, Vec4(1, 0, 0));

            }

            death->position += (earth->position - death->position) * 0.05f;

            static Entity *explode1 = NULL;
            static Entity *explode2 = NULL;
            if(death->position.distance(earth->position) < 10) {
                if(!explode1) {
                    explode1 = scene->addEntity("explode");
                    explode2 = scene->addEntity("explode");
                    explode1->rotation.rotate(M_PI, Vec4(1, 0, 0));
                    explode2->rotation.rotate(M_PI/2.0f, Vec4(1, 0, 0));
                    Mix_PlayChannelTimed(-1, boom, 0, -1);
                }
                explode1->scale += Vec4(1.0, 1.0, 1.0, 0);
                explode2->scale += Vec4(2.0, 2.0, 2.0, 0);

                earth->scale.scale(0.5);
                death->scale.scale(0.5);
                fade += 0.03;
            }
        }
    }

    if(state == GAME || state == END) {
        static int tick = 0;
        tick++;
        if(tick % 50 == 0) {
            life->Poll();
        }

        moon1->position = Vec4(MOON_ORBIT1 * cos((tick+20) / 401.0f),
                              sin((tick+20) / 97.0f) * 5.0f,
                              MOON_ORBIT1 * sin((tick+20) / 401.0f));

        moon2->position = Vec4(MOON_ORBIT2 * cos(tick/ 301.0f),
                              sin(tick / 47.0f) * 5.0f,
                              MOON_ORBIT2 * sin(tick / 301.0f));

        moon1->rotation = Mat4();
        moon1->rotation.rotate(tick / 161.0f, Vec4(0, 1, 0));

        moon2->rotation = Mat4();
        moon2->rotation.rotate(-tick / 101.0f, Vec4(0, 1, 0));
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
    } else if (state == GAME || state == END) {
        Mat4 vmat;
        Mat4 sky_vmat;
        Vec4 side = player_forward.cross(player_vector).normalized();
        vmat.setXRow(side);
        vmat.setYRow(player_vector);
        vmat.setZRow(player_forward);

        sky_vmat = Mat4().scaled(10, 10, 10) * vmat;

        vmat.translate(Vec4(0, -115, 0));

        static float end_gaze = 0.0f;
        if(state == END) {
            if(end_gaze > -M_PI/2.0f)
                end_gaze -= 0.010;
            else everyOneDies = true;
        }

        Mat4 gaze;
        gaze.rotate(0.6+end_gaze, Vec4(1, 0, 0));
        vmat = gaze * vmat;

        sky_vmat = gaze * sky_vmat;

        drawDevice->runSkyboxProgram(
                scene->getTexture("sky-day"),
                scene->getTexture("sky-night"),
                sky_vmat, Vec4());

        scene->draw(drawDevice, vmat);

        for(int j = 0; j < life->GetHeight(); j++) {
            for(int i = 0; i < life->GetWidth(); i++) {
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

    drawDevice->runOutputProgram(fade);
    window->SwapBuffers();
}

void tick() {
    input();
    update();
    draw();
}

int main(int argc, char **argv) {
    init();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(tick, 0, 1);
#else
    while(running) {
        tick();
        usleep(10);
    }
#endif
    return 0;
}
