#include "PacMan.hpp"
#include "Ghost.hpp"
#include <iostream>
#include "TextureManager.hpp"
#include "Game.hpp"


void PacMan::initVariables() {
    this->movementSpeed = 2.0f;
    this->movementDirection = Direction::Left; // Default direction
    this->desiredDirection = Direction::Left;  // Default desired direction
    this->currentFrame = 0;
    this->animationSpeed = 0.3f;
    this->deathAnimationSpeed = 0.17f;
    this->frameTime = 0.f;
}

void PacMan::initSprite() {
    const sf::Texture& texture = TextureManager::getInstance().getTexture("Media.xcassets/Spritesheet.imageset/Spritesheet.png");
    this->sprite.setTexture(texture);

    // Set the initial texture rectangle for Pac-Man facing right
    this->sprite.setTextureRect(sf::IntRect(102, 500, 13, 13)); // Pac-Man facing right with a closed mouth

    // Scale the sprite to match the desired radius
    float scaleFactor = 13.1f * 2 / 13.0f; // 13.1f radius, so 26.2f diameter
    this->sprite.setScale(scaleFactor, scaleFactor);
}

PacMan::PacMan(float x, float y) {
    this->initVariables();
    this->initSprite();
    
    // Set the initial position of the shape
    this->sprite.setPosition(x, y);
}

PacMan::~PacMan() {}

sf::Vector2f PacMan::getPosition() const {
    return this->sprite.getPosition();
}

void PacMan::handleKeyPress(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Left:
            this->desiredDirection = Direction::Left;
            break;
        case sf::Keyboard::Right:
            this->desiredDirection = Direction::Right;
            break;
        case sf::Keyboard::Up:
            this->desiredDirection = Direction::Up;
            break;
        case sf::Keyboard::Down:
            this->desiredDirection = Direction::Down;
            break;
        default:
            break;
    }
}

void PacMan::updateWindowBoundsCollision(const sf::RenderTarget* target) {
    // Left tunnel teleportation
    if (this->sprite.getGlobalBounds().left + this->sprite.getGlobalBounds().width < 0.f) {
        this->sprite.setPosition(target->getSize().x - this->sprite.getGlobalBounds().width, this->sprite.getPosition().y);
    }
    // Right tunnel teleportation
    else if (this->sprite.getGlobalBounds().left > target->getSize().x) {
        this->sprite.setPosition(0.f - this->sprite.getGlobalBounds().width, this->sprite.getPosition().y);
    }
}

void PacMan::updateDirection(const std::vector<std::string>& map, int tileSize) {
    sf::Vector2f movement(0.f, 0.f);
    switch (this->desiredDirection) {
        case Direction::Left: movement.x = -this->movementSpeed; break;
        case Direction::Right: movement.x = this->movementSpeed; break;
        case Direction::Up: movement.y = -this->movementSpeed; break;
        case Direction::Down: movement.y = this->movementSpeed; break;
        case Direction::None: break;
    }

    sf::FloatRect futureBounds = this->sprite.getGlobalBounds();
    futureBounds.left += movement.x;
    futureBounds.top += movement.y;

    if (!this->checkWallCollision(futureBounds, map, tileSize)) {
        float centerOffset = tileSize / 2 - 13.1f;
        switch (this->desiredDirection) {
            case Direction::Left:
            case Direction::Right:
                this->sprite.setPosition(this->sprite.getPosition().x, std::round(this->sprite.getPosition().y / tileSize) * tileSize + centerOffset);
                break;
            case Direction::Up:
            case Direction::Down:
                this->sprite.setPosition(std::round(this->sprite.getPosition().x / tileSize) * tileSize + centerOffset, this->sprite.getPosition().y);
                break;
            case Direction::None: break;
        }
        this->movementDirection = this->desiredDirection;
    }
}

Direction PacMan::getCurrentDirection() const {
    return this->movementDirection;
}

void PacMan::update(const sf::RenderTarget* target, const std::vector<std::string>& map, std::vector<Pellet>& pellets, std::vector<Energizer>& energizers, std::vector<std::unique_ptr<Ghost>>& ghosts, Game* game) {
    
    if (game->getEndGame()) {
        // Stop Pac-Man's movement and play death animation
        this->updateDeathAnimation();
        return;
    }
    
    this->updateDirection(map, game->getTileSize());

    sf::Vector2f movement(0.f, 0.f);
    switch (this->movementDirection) {
        case Direction::Left: movement.x = -this->movementSpeed; break;
        case Direction::Right: movement.x = this->movementSpeed; break;
        case Direction::Up: movement.y = -this->movementSpeed; break;
        case Direction::Down: movement.y = this->movementSpeed; break;
        case Direction::None: break;
    }

    sf::FloatRect futureBounds = this->sprite.getGlobalBounds();
    futureBounds.left += movement.x;
    futureBounds.top += movement.y;

    bool wallCollision = this->checkWallCollision(futureBounds, map, game->getTileSize());
    if (!wallCollision) {
        this->sprite.move(movement);
    }

    this->updateWindowBoundsCollision(target);

    // Handle pellet, energizer, and ghost collisions
    this->checkPelletCollision(pellets);
    this->checkEnergizerCollision(energizers, ghosts, game);
    this->checkGhostCollision(ghosts, game);

    // Update animation frame
    this->updateAnimation(wallCollision);
}

void PacMan::updateDeathAnimation() {
    this->frameTime += this->deathAnimationSpeed;
    if (this->frameTime >= 1.f) {
        this->frameTime = 0.f;
        this->currentFrame++;

        if (this->currentFrame >= 12) {
            this->currentFrame = 11;  // Keep the last frame displayed once the animation ends
        }

        int frameX = 152 + (this->currentFrame * 17);
        int frameY = 501;

        this->sprite.setTextureRect(sf::IntRect(frameX, frameY, 15, 11));
    }
}

void PacMan::updateAnimation(bool wallCollision) {
    
    if(wallCollision){
        int frameX = 0, frameY = 0;
        // Set the partially open mouth frame for the last direction faced
        switch (this->movementDirection) {
            case Direction::Right: frameX = 118; frameY = 534; break;
            case Direction::Left:  frameX = 119; frameY = 500; break;
            case Direction::Up:    frameX = 119; frameY = 517; break;
            case Direction::Down:  frameX = 119; frameY = 550; break;
            default: break;
        }
        
        this->sprite.setTextureRect(sf::IntRect(frameX, frameY, 13, 13));
    }
    else{
        this->frameTime += this->animationSpeed;
        if (this->frameTime >= 1.f) {
            this->frameTime = 0.f;
            this->currentFrame = (this->currentFrame + 1) % 3; // Cycle through the 3 frames for each direction
            
            int frameX = 102; // Default X-coordinate for left-facing closed mouth Pac-Man
            int frameY = 500; // Default Y-coordinate for left-facing closed mouth Pac-Man
            
            switch (this->movementDirection) {
                case Direction::Right: frameX = 101 + (this->currentFrame * 17); frameY = 534; break;
                case Direction::Left:frameX = 102 + (this->currentFrame * 17); frameY = 500; break;
                case Direction::Up:frameX = 102 + (this->currentFrame * 17); frameY = 517; break;
                case Direction::Down:frameX = 102 + (this->currentFrame * 17); frameY = 550; break;
                default: break;
            }
            
            this->sprite.setTextureRect(sf::IntRect(frameX, frameY, 13, 13));
        }
    }
}

bool PacMan::checkWallCollision(const sf::FloatRect& futureBounds, const std::vector<std::string>& map, int tileSize) {
    for (size_t y = 0; y < map.size(); ++y) {
        for (size_t x = 0; x < map[y].size(); ++x) {
            if (map[y][x] == '#' || map[y][x] == '-') {
                sf::FloatRect wallBounds(x * tileSize, y * tileSize, tileSize, tileSize);
                if (futureBounds.intersects(wallBounds)) {
                    return true;
                }
            }
        }
    }
    return false;
}

void PacMan::checkPelletCollision(std::vector<Pellet>& pellets) {
    
    for (auto& pellet : pellets) {
        if (!pellet.isEaten() && this->sprite.getGlobalBounds().intersects(pellet.getShape().getGlobalBounds())) {
            pellet.setEaten(true);
        }
    }
}

void PacMan::checkEnergizerCollision(std::vector<Energizer>& energizers, std::vector<std::unique_ptr<Ghost>>& ghosts, Game* game) {
    for (auto& energizer : energizers) {
        if (!energizer.isEaten() && this->sprite.getGlobalBounds().intersects(energizer.getShape().getGlobalBounds())) {
            energizer.setEaten(true);
            // Set frightened mode for all ghosts
            for (auto& ghost : ghosts) {
                ghost->setFrightened(true);
                game->resetGhostEatenCount();
            }
        }
    }
}

void PacMan::checkGhostCollision(std::vector<std::unique_ptr<Ghost>>& ghosts, Game* game) {
    for (auto& ghost : ghosts) {
        if (this->shrinkedBounds().intersects(ghost->shrinkedBounds())) {
            if (ghost->getIsFrightened()) {
                // If the ghost is in frightened mode, send it back to the ghost house
                ghost->setReturnToGhostHouse(true);
                ghost->setEaten(true);
            } else if (!ghost->getIsReturningToHouse()) {
                // If the ghost is not in frightened mode, end the game
                std::cout << "Game Over! Pac-Man was caught by a ghost." << std::endl;
                // Handle game over logic here, e.g., resetting the game, showing a game over screen, etc.
                game->setEndGame(true);
            }
        }
    }
}

sf::FloatRect PacMan::shrinkedBounds() const {
    const float shrinkFactor = 0.7f; // Adjust the factor as needed (1.0 = original size)
    sf::FloatRect originalBounds = this->sprite.getGlobalBounds();
    
    float widthShrink = originalBounds.width * (1 - shrinkFactor) / 2;
    float heightShrink = originalBounds.height * (1 - shrinkFactor) / 2;

    return sf::FloatRect(
        originalBounds.left + widthShrink,
        originalBounds.top + heightShrink,
        originalBounds.width * shrinkFactor,
        originalBounds.height * shrinkFactor
    );
}

void PacMan::render(sf::RenderTarget* target) {
    target->draw(this->sprite);
}
