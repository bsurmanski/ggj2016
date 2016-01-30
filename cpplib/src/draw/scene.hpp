#ifndef _DRAW_SCENE_HPP
#define _DRAW_SCENE_HPP

#include "common/input.hpp"
#include "common/string.hpp"
#include "draw/gl/mesh.hpp"
#include "entity/entity.hpp"

#include <map>
#include <vector>

class Scene {
    std::map<String, GL::Mesh*> meshes;
    std::map<String, GL::Texture*> textures;
    std::vector<Entity*> entities;

    public:
    void registerMesh(const String &k, GL::Mesh *m);
    void registerTexture(const String &k, GL::Texture *t);
    Entity *addEntity(const String &k);
};

void loadScn(Scene *s, Input *in);

#endif
