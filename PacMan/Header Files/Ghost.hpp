#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <limits>
#include "Direction.hpp"

class PacMan;

namespace std {
    template<>
    struct hash<sf::Vector2i> {
        std::size_t operator()(const sf::Vector2i& v) const {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
        }
    };
}

struct Node {
    sf::Vector2i position;
    Direction direction;
};

class Ghost {
protected:
    int tileSize;
    Direction movementDirection;
    sf::Texture texture;
    sf::Sprite sprite;
    
    bool eaten = false;
    
    int currentFrame;
    float animationSpeed;
    float frameTime;

    int spriteYOffset;
    sf::Vector2f position;
    float movementSpeed;
    
    float normalSpeed = 2.0f;
    float frightenedSpeed = 1.0f;
    float returnToHouseSpeed = 4.0f;
    
    bool isChaseMode;
    bool released;
    bool isGhostInHouse;
    sf::Vector2i currentTile;
    Direction previousDirection;
    int cornerLoopIndex;
    std::vector<Direction> path;
    bool isFrightened;
    bool returnToGhostHouse;
    
    sf::Clock frightenedClock; // Timer for frightened mode
    const sf::Time frightenedDuration = sf::seconds(8); // Duration for frightened mode

    void initVariables(float x, float y, int tileSize);
    void initSprite(float x, float y);
    
    virtual std::vector<sf::Vector2i> getCornerLoop() const = 0;
    bool checkWallCollision(const sf::FloatRect& futureBounds, const std::vector<std::string>& map) const;
    void handleTunnelTeleportation(const sf::RenderTarget* target);
    virtual sf::Vector2i getTargetTile(const sf::Vector2f& pacManPosition, Direction pacManDirection, const sf::Vector2f& blinkyPosition, const std::vector<std::string>& map) = 0;
    void loopAroundCorner(const std::vector<std::string>& map);
    bool isInGhostHouse(const std::vector<std::string>& map);
    bool isInTunnel(const std::vector<std::string>& map);
    bool isGhostFullyInTile(const sf::FloatRect& ghostBody, const sf::Vector2i& tilePosition);
    std::vector<Direction> findPathBFS(const sf::Vector2i& start, const sf::Vector2i& target, const std::vector<std::string>& map);
    void updateNormalAnimation();
    void updateReturnToHomeAnimation();
    void updateFrightenedAnimation();
    

public:
    Ghost(float x, float y, int spriteYOffset, int tileSize);
    virtual ~Ghost();
    
    bool isEaten() const { return this->eaten; }
    void setEaten(bool eaten) { this->eaten = eaten; }
    
    void switchMode();
    void setFrightened(bool state);
    void setReturnToGhostHouse(bool state);
    bool getIsFrightened() const;
    bool getIsChaseMode() const;
    bool getIsReturningToHouse() const;
    bool isReleased() const;
    void release();
    sf::FloatRect shrinkedBounds() const;
    bool getIsGhostInHouse() const { return isGhostInHouse; }
    sf::Vector2f getPosition() const;
    sf::FloatRect getGlobalBounds() const;
    Direction getPacManDirection(const PacMan& pacMan) const;
    void update(const sf::Vector2f& pacManPosition, Direction pacManDirection, const sf::Vector2f& blinkyPosition, const std::vector<std::string>& map, int dotsRemaining, const sf::RenderTarget* target, bool modeChanged, int chaseCounter, int scatterCounter, int ghostEatenCount);
    void render(sf::RenderTarget* target);
};
