#include "Game.hpp"
#include "Utils/TextureManager.hpp"
#include "Map.hpp"
#include "ECS/Components.hpp"
#include "ECS/Collider.h"
#include "Utils/AssetManager.hpp"

SDL_Renderer *Game::renderer = nullptr;
SDL_Event Game::event;

bool Game::isRunning = true;

SDL_Rect Game::camera = {0, 0, 780, 600};

Manager manager;
AssetManager *Game::assets = new AssetManager(&manager);

auto &player(manager.addEntity());
auto &IA(manager.addEntity());
auto &wall(manager.addEntity());

SDL_Rect destR;
const int offset = 200;

Game::Game() {}

Game::~Game() {}

void Game::init(const char *title, int xpos, int ypos, int width, int height, bool fullScreen)
{

  if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
  {
    cout << "The iniciatilize is fine!" << endl;

    window = SDL_CreateWindow(title, xpos, ypos, width, height, fullScreen ? SDL_WINDOW_FULLSCREEN : 0);

    if (window)
    {
      cout << "Window Created!" << endl;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (renderer)
    {
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      cout << "Renderer Created!" << endl;
      isRunning = true;
    }
  }
  else
  {
    isRunning = false;
  }
}

SDL_Texture *TileComponent::grass = NULL;
SDL_Texture *TileComponent::water = NULL;
SDL_Texture *TileComponent::brick = NULL;
SDL_Texture *TileComponent::ground = NULL;
SDL_Texture *TileComponent::sky = NULL;


void Game::start()
{

  TileComponent::PreLoadTextures();
  Map::loadMap("../resources/Map.txt", 20, 26);
  Map::loadColliders("../resources/Colliders.txt", 20, 26);

  player.addComponent<TransformComponent>(500.0f, 1020.0f, 64, 64, 1);
  Animation idle = Animation(74, 74, 3, 400, "../resources/Characters/wizard/idle.png");
  Animation walk = Animation(78, 74, 4, 300, "../resources/Characters/wizard/walk.png");
  map<const char *, Animation> playerAnims;
  playerAnims.emplace("Idle", idle);
  playerAnims.emplace("Walk", walk);
  player.addComponent<SpriteComponent>(playerAnims);
  player.addComponent<KeyboardController>();
  player.addComponent<ColliderComponent>("player");
  player.addGroup(groupPlayers);

  IA.addComponent<IAAgentComponent>(Vector2D(400.0f, 1020.0f), &player.getComponent<TransformComponent>().position, 3, "../resources/Characters/player.png", 2);
  IA.addGroup(groupIA);

  Game::assets->CreateProjectile(Vector2D(500, 900), "../resources/Projectiles/soccer.png");
  Game::assets->CreateProjectile(Vector2D(400, 900), "../resources/Projectiles/soccer.png");
  Game::assets->CreateProjectile(Vector2D(600, 900), "../resources/Projectiles/soccer.png");
  Game::assets->CreateProjectile(Vector2D(700, 900), "../resources/Projectiles/soccer.png");
  Game::assets->CreateProjectile(Vector2D(300, 900), "../resources/Projectiles/soccer.png");
}

auto &tiles(manager.getGroup(Game::groupMap));
auto &players(manager.getGroup(Game::groupPlayers));
auto &enemies(manager.getGroup(Game::groupEnemies));
auto &colliders(manager.getGroup(Game::groupColliders));
auto &projectiles(manager.getGroup(Game::groupProjectiles));
auto &IAs(manager.getGroup(Game::groupIA));

void Game::render()
{
  SDL_RenderClear(renderer);
  for (auto &t : tiles)
    t->draw();
  for (auto &p : players)
    p->draw();
  for (auto &e : enemies)
    e->draw();
  for (auto &i : projectiles)
    i->draw();
  for (auto &j : IAs)
    j->draw();
  SDL_RenderPresent(renderer);
}

void Game::clean()
{
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
  cout << "Game Cleaned" << endl;
}

void Game::handleEvents()
{
  SDL_PollEvent(&event);

  switch (event.type)
  {
  case SDL_QUIT:
    isRunning = false;
    break;

  default:
    break;
  }
}

bool Game::running() { return isRunning; }

void Game::addCol(int ID, int x, int y)
{
  auto &col(manager.addEntity());
  col.addComponent<TransformComponent>(x, y, 64, 64, ID);
  col.addComponent<ColliderComponent>("Collider");
  col.addGroup(groupColliders);
}

void Game::addTile(int ID, int x, int y)
{
  auto &tile(manager.addEntity());
  tile.addComponent<TileComponent>(x, y, 64, 64, ID);
  tile.addGroup(groupMap);
}

void Game::update()
{

  SDL_Rect playerCol = player.getComponent<ColliderComponent>().collider;
  Vector2D playerPos = player.getComponent<TransformComponent>().position;

  manager.refresh();
  manager.update();

  for (auto &c : colliders)
  {
    SDL_Rect cCol = c->getComponent<ColliderComponent>().collider;

    if (Collision::AABB(cCol, playerCol, 5))
    {
      player.getComponent<TransformComponent>().position = playerPos;
    }

    for (auto &p : projectiles)
    {
      if (Collision::AABB(cCol, p->getComponent<ColliderComponent>().collider, 5))
      {
        std::cout << "Hit!" << std::endl;
        p->destroy();
      }
    }
  }

  camera.x = player.getComponent<TransformComponent>().position.x - offset;
  camera.y = player.getComponent<TransformComponent>().position.y - offset;

  if (camera.x < 0)
    camera.x = 0;
  if (camera.y < 0)
    camera.y = 0;
  if (camera.x > camera.w)
    camera.x = camera.w;
  if (camera.y > camera.h)
    camera.y = camera.h;

  SDL_UpdateWindowSurface(window);
}