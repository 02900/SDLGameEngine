#ifndef AssetManager_hpp
#define AssetManager_hpp

#include <map>
#include <string>
#include "TextureManager.hpp"
#include "../Math/Vector2D.hpp"
#include "../ECS/ECS.hpp"
#include "../ECS/Components.hpp"
#include "../Game.hpp"

class AssetManager
{
public:
    AssetManager(Manager *man);
    ~AssetManager();

    void addTexture(std::string ID, const char *path);
    void CreateProjectile(Vector2D pos, const char *path);
    SDL_Texture *getTexture(std::string ID);

private:
    Manager *manager;
    map<std::string, SDL_Texture *> textures;
};

#endif /* AssetManager_hpp */
