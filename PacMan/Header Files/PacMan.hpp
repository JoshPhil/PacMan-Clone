#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <string>
#include "Direction.hpp"
#include "Pellet.hpp"
#include "Energizer.hpp"

class Game;
class Ghost;

class PacMan {
private:
    sf::CircleShape shape;
    sf::Texture texture;
    sf::Sprite sprite;
    float movementSpeed;
    int currentFrame;
    float animationSpeed;
    float deathAnimationSpeed;
    float frameTime;
    
    Direction movementDirection;
    Direction desiredDirection;

    void initSprite();
    void initVariables();
    bool checkWallCollision(const sf::FloatRect& futureBounds, const std::vector<std::string>& map, int tileSize);
    void updateDirection(const std::vector<std::string>& map, int tileSize);
    void checkPelletCollision(std::vector<Pellet>& pellets);
    void checkEnergizerCollision(std::vector<Energizer>& energizers, std::vector<std::unique_ptr<Ghost>>& ghosts, Game* game);
    void checkGhostCollision(std::vector<std::unique_ptr<Ghost>>& ghosts, Game* game);
    void updateDeathAnimation();
    void updateAnimation(bool wallCollision);
    sf::FloatRect shrinkedBounds() const;

public:
    PacMan(float x = 0.f, float y = 0.f);
    virtual ~PacMan();
    
    
    sf::Vector2f getPosition() const;
    
    Direction getCurrentDirection() const;
    void handleKeyPress(sf::Keyboard::Key key);
    void updateWindowBoundsCollision(const sf::RenderTarget* target);
    void update(const sf::RenderTarget* target, const std::vector<std::string>& map, std::vector<Pellet>& pellets, std::vector<Energizer>& energizers, std::vector<std::unique_ptr<Ghost>>& ghosts, Game* game);
    void render(sf::RenderTarget* target);
};
