#ifndef IAAgentComponent_hpp
#define IAAgentComponent_hpp

#include "Components.hpp"
#include "../Math/Vector2D.hpp"
#include "ECS.hpp"

#include <SDL.h>
#include "../Utils/TextureManager.hpp"

class IAAgentComponent : public Component
{
public:
    Vector2D position;
    int height = 32;
    int width = 32;
    int scale = 2;

    int minDistance = 100;

    IAAgentComponent(Vector2D position, Vector2D *target, const float sp, const char *path, int beha) : maxSpeed(sp), behavior(beha)
    {
        this->position = position;
        this->target = target;

        scrRect.w = 32;
        scrRect.h = 32;
        setTexture(path);
    }

    ~IAAgentComponent()
    {
        SDL_DestroyTexture(texture);
    }

    Vector2D flee()
    {
        Vector2D direction;
        direction.x = position.x - target->x;
        direction.y = position.y - target->y;

        if (direction.magnitude() > minDistance)
            return direction.zero();

        direction = direction * (1 / direction.magnitude());

        return direction;
    }

    Vector2D seek()
    {
        Vector2D direction;
        direction.x = target->x - position.x;
        direction.y = target->y - position.y;

        if (direction.magnitude() < minDistance)
            return direction.zero();

        direction = direction * (1 / direction.magnitude());
        return direction;
    }

    void update() override
    {
        switch(behavior){
            case 1:
                velocity = flee() * maxSpeed;
            break;
            
            case 2:
                velocity = seek() * maxSpeed;
            break;
        }

        position.x += velocity.x;
        position.y += velocity.y;

        destRect.x = static_cast<int>(position.x) - Game::camera.x;
        destRect.y = static_cast<int>(position.y) - Game::camera.y;
        destRect.w = width * scale;
        destRect.h = height * scale;
    }

    //Load Textures

    void setTexture(const char *path)
    {
        texture = TextureManager::loadTexture(path);
    }

    void draw() override
    {
        TextureManager::Draw(texture, destRect);
    }

private:
    Vector2D velocity;
    Vector2D *target;
    int behavior;

    const float maxSpeed;

    SDL_Texture *texture;
    SDL_Rect destRect;
    SDL_Rect scrRect;
};

#endif /* IAAgentComponent_hpp */
