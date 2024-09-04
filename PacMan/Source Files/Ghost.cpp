#include "Ghost.hpp"
#include "PacMan.hpp"
#include "TextureManager.hpp"


Ghost::Ghost(float x, float y, int spriteYOffset, int tileSize) {
    this->initVariables(x, y, tileSize);
    this->spriteYOffset = spriteYOffset;
    this->initSprite(x, y);
}


Ghost::~Ghost() {}


void Ghost::initVariables(float x, float y, int tileSize) {
    this->tileSize = tileSize;
    this->movementSpeed = 2.f;
    this->isChaseMode = false;
    this->isGhostInHouse = true;
    this->released = false;
    this->currentTile = sf::Vector2i(static_cast<int>(x / 28), static_cast<int>(y / 28));
    this->previousDirection = Direction::None;
    this->cornerLoopIndex = 0;
    this->path.clear();
    this->isFrightened = false;
    this->frightenedClock.restart();
    this->returnToGhostHouse = false;
    this->currentFrame = 0;
    this->animationSpeed = 0.1f; // Speed of animation
    this->frameTime = 0.f;
    std::srand(static_cast<unsigned>(std::time(nullptr))); // Seed for random direction
}


void Ghost::initSprite(float x, float y) {
    const sf::Texture& texture = TextureManager::getInstance().getTexture("Media.xcassets/Spritesheet.imageset/Spritesheet.png");
    this->sprite.setTexture(texture);

    // Set the initial texture rectangle based on the Y offset
    this->sprite.setTextureRect(sf::IntRect(169, spriteYOffset, 14, 14)); // Example for right-facing frame

    // Scale the sprite to match the desired size
    float scaleFactor = 13.1f * 2 / 14.0f; // Adjust this scale factor if necessary
    this->sprite.setScale(scaleFactor, scaleFactor);

    // Set the initial position of the sprite
    this->sprite.setPosition(x, y);
}


bool Ghost::isReleased() const {
    return released;
}


void Ghost::release() {
    released = true;
}


Direction Ghost::getPacManDirection(const PacMan& pacMan) const {
    return pacMan.getCurrentDirection();
}


sf::Vector2f Ghost::getPosition() const {
    return this->sprite.getPosition();
}


sf::FloatRect Ghost::getGlobalBounds() const {
    return this->sprite.getGlobalBounds();
}


bool Ghost::getIsChaseMode() const {
    return this->isChaseMode;
}


bool Ghost::getIsFrightened() const{
    return this->isFrightened;
}


bool Ghost::getIsReturningToHouse() const{
    return this->returnToGhostHouse;
}


void Ghost::setFrightened(bool state) {
    isFrightened = state;
    if (state) {
        frightenedClock.restart(); // Restart the clock when entering frightened mode
    }
}


void Ghost::setReturnToGhostHouse(bool state) {
    this->returnToGhostHouse = state;
}


void Ghost::switchMode() {
    isChaseMode = !isChaseMode;
}


bool Ghost::isInGhostHouse(const std::vector<std::string>& map) {
    if ((currentTile.y == 9) && (currentTile.x >= 9 && currentTile.x <= 11)) {
        this->isGhostInHouse = true;
        return true;
    } else {
        this->isGhostInHouse = false;
        return false;
    }
}


bool Ghost::isInTunnel(const std::vector<std::string>& map) {
    // Check if the ghost is in the tunnel area or just outside the bounds
    if ((currentTile.y == 9) && ( (currentTile.x <= 4) || (currentTile.x >= 16)) )
    {
        return true;
    } else {
        return false;
    }
}


sf::FloatRect Ghost::shrinkedBounds() const {
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


bool Ghost::isGhostFullyInTile(const sf::FloatRect& ghostBody, const sf::Vector2i& tilePosition) {
    // Calculate tile boundaries
    float tileLeft = tilePosition.x * this->tileSize;
    float tileTop = tilePosition.y * this->tileSize;
    float tileRight = tileLeft + this->tileSize;
    float tileBottom = tileTop + this->tileSize;

    // Check if the ghost's bounding box is fully within the tile
    return ghostBody.left >= tileLeft &&
           ghostBody.top >= tileTop &&
           ghostBody.left + ghostBody.width <= tileRight &&
           ghostBody.top + ghostBody.height <= tileBottom;
}


void Ghost::render(sf::RenderTarget* target) {
    target->draw(this->sprite);
}


std::vector<Direction> Ghost::findPathBFS(const sf::Vector2i& start, const sf::Vector2i& target, const std::vector<std::string>& map) {
    std::unordered_map<sf::Vector2i, sf::Vector2i> cameFrom;
    std::unordered_map<sf::Vector2i, Direction> directionFrom;
    std::queue<sf::Vector2i> frontier;
    std::unordered_map<sf::Vector2i, bool> visited;

    cameFrom[start] = start;
    frontier.push(start);
    visited[start] = true;

    while (!frontier.empty()) {
        sf::Vector2i current = frontier.front();
        frontier.pop();

        if (current == target) {
            break;
        }

        std::vector<std::pair<sf::Vector2i, Direction>> neighbors = {
            {{current.x - 1, current.y}, Direction::Left},
            {{current.x + 1, current.y}, Direction::Right},
            {{current.x, current.y - 1}, Direction::Up},
            {{current.x, current.y + 1}, Direction::Down}
        };

        for (const auto& neighbor : neighbors) {
            sf::Vector2i next = neighbor.first;
            Direction direction = neighbor.second;

            // Check bounds
            if (next.y >= 0 && next.y < map.size() &&
                next.x >= 0 && next.x < map[next.y].size()) {
                
                // Check if the tile is not a wall and has not been visited
                if (map[next.y][next.x] != '#' && visited.find(next) == visited.end()) {
                    frontier.push(next);
                    visited[next] = true;
                    cameFrom[next] = current;
                    directionFrom[next] = direction;
                }
            }
        }
    }

    std::vector<Direction> path;
    sf::Vector2i current = target;

    if (cameFrom.find(current) == cameFrom.end()) {
        return {}; // No path found
    }
    
    while (current != start) {
        if (cameFrom.find(current) == cameFrom.end()) {
            return {}; // No path found
        }

        path.push_back(directionFrom[current]);
        current = cameFrom[current];
    }

    std::reverse(path.begin(), path.end());

    return path;
}


bool Ghost::checkWallCollision(const sf::FloatRect& futureBounds, const std::vector<std::string>& map) const {
    const float margin = 0.5f; // Margin to account for small discrepancies

    // Expanded bounds to add a small margin around the future position
    sf::FloatRect expandedBounds = futureBounds;
    expandedBounds.left -= margin;
    expandedBounds.top -= margin;
    expandedBounds.width += 2 * margin;
    expandedBounds.height += 2 * margin;

    for (size_t y = 0; y < map.size(); ++y) {
        for (size_t x = 0; x < map[y].size(); ++x) {
            char cell = map[y][x];
            if (cell == '#' || cell == '0' ||
                ((!this->isGhostInHouse && !this->isFrightened && !this->returnToGhostHouse) && cell == '-') ||
                ((!this->isGhostInHouse && this->isFrightened) && cell == '-') ) {
                sf::FloatRect wallBounds(x * this->tileSize, y * this->tileSize, this->tileSize, this->tileSize);
                if (expandedBounds.intersects(wallBounds)) {
                    return true;
                }
            }
        }
    }
    return false;
}


void Ghost::handleTunnelTeleportation(const sf::RenderTarget* target) {
    const sf::FloatRect bounds = this->sprite.getGlobalBounds();

    // Left tunnel teleportation
    if (bounds.left + bounds.width < 0.f) {
        this->sprite.setPosition(target->getSize().x, this->sprite.getPosition().y);
    }
    // Right tunnel teleportation
    else if (bounds.left > target->getSize().x) {
        this->sprite.setPosition(0.f - bounds.width, this->sprite.getPosition().y);
    }
}


void Ghost::update(const sf::Vector2f& pacManPosition, Direction pacManDirection, const sf::Vector2f& blinkyPosition, const std::vector<std::string>& map, int dotsRemaining, const sf::RenderTarget* target, bool modeChanged, int chaseCounter, int scatterCounter, int ghostEatenCount) {
    
    if (eaten) {
        // If the ghost is eaten, display the corresponding points
        int pointsX = 0;
        switch (ghostEatenCount) {
            case 1: pointsX = 440; break; // 200 points
            case 2: pointsX = 457; break; // 400 points
            case 3: pointsX = 474; break; // 800 points
            case 4: pointsX = 491; break; // 1600 points
        }
        
        this->sprite.setTextureRect(sf::IntRect(pointsX, 566, 16, 16));
        return; // Skip the rest of the update logic if showing points
    }
    
    if (isFrightened && frightenedClock.getElapsedTime() >= frightenedDuration) {
        setFrightened(false);
    }
    
    Direction moveDirection = Direction::None;
    Direction chosenDirection = Direction::None;
    sf::FloatRect ghostHead = this->sprite.getGlobalBounds();
    sf::Vector2i potentialCurrentTile(static_cast<int>(ghostHead.left / 28), static_cast<int>(ghostHead.top / 28));
    sf::Vector2i targetTile;
    
    this->handleTunnelTeleportation(target);
    
    if (isGhostFullyInTile(ghostHead, potentialCurrentTile)) {
        currentTile = potentialCurrentTile;
    }
    
    
    if(isInTunnel(map)){
        this->movementSpeed = frightenedSpeed; // slow down in tunnel
    }
    else{
        if (isGhostFullyInTile(ghostHead, potentialCurrentTile)) {
            this->movementSpeed = normalSpeed;
        }
    }
    
    if (returnToGhostHouse) {
        targetTile = sf::Vector2i(10, 9);
        isFrightened = false;
        if (this->isInGhostHouse(map)) {
            returnToGhostHouse = false;
            if (isGhostFullyInTile(ghostHead, potentialCurrentTile)) {
                this->movementSpeed = normalSpeed;
            }
        } else {
            if (isGhostFullyInTile(ghostHead, potentialCurrentTile)) {
                this->movementSpeed = returnToHouseSpeed;
            }
            
        }
    } else if (isFrightened) {
        if (this->isInGhostHouse(map)) {
            targetTile = sf::Vector2i(10, 7);
        }
        this->movementSpeed = frightenedSpeed;
    } else {
        if (this->isInGhostHouse(map)) {
            targetTile = sf::Vector2i(10, 7);
        } else {
            if (isGhostFullyInTile(ghostHead, potentialCurrentTile)) {
                this->movementSpeed = normalSpeed;
            }
            if (!isChaseMode) {
                auto cornerLoop = getCornerLoop();
                if (currentTile == cornerLoop[cornerLoopIndex]) {
                    cornerLoopIndex = (cornerLoopIndex + 1) % cornerLoop.size();
                }
                targetTile = cornerLoop[cornerLoopIndex];
            } else {
                targetTile = getTargetTile(pacManPosition, pacManDirection, blinkyPosition, map);
            }
        }
    }
    
    auto path = findPathBFS(currentTile, targetTile, map);
    
    if (isFrightened && !this->isInGhostHouse(map) && !returnToGhostHouse) {
        // Random direction in frightened mode
        std::vector<Direction> possibleDirections = { Direction::Left, Direction::Right, Direction::Up, Direction::Down };
        std::vector<Direction> validDirections;
        
        // Collect valid directions
        for (const auto& direction : possibleDirections) {
            if (direction == getOppositeDirection(previousDirection)) continue; // Skip reverse direction
            
            sf::Vector2f futurePosition = this->sprite.getPosition();
            switch (direction) {
                case Direction::Left: futurePosition.x -= this->movementSpeed; break;
                case Direction::Right: futurePosition.x += this->movementSpeed; break;
                case Direction::Up: futurePosition.y -= this->movementSpeed; break;
                case Direction::Down: futurePosition.y += this->movementSpeed; break;
                case Direction::None: break;
            }
            
            sf::FloatRect futureBounds(futurePosition, ghostHead.getSize());
            if (!checkWallCollision(futureBounds, map)) {
                validDirections.push_back(direction);
            }
        }
        
        if (!validDirections.empty()) {
            moveDirection = validDirections[std::rand() % validDirections.size()];
        }
    }
    else {
        // Normal movement
        if (!path.empty()) {
            Direction nextDirection = path.front();
            
            // Check if the path suggests a reverse direction
            Direction oppositeDirection = getOppositeDirection(previousDirection);
            if (nextDirection == oppositeDirection && !isGhostInHouse && chaseCounter != 0 && scatterCounter != 0) {
                // Move in a random direction if reverse direction is suggested and not during mode change
                std::vector<Direction> possibleDirections = { Direction::Left, Direction::Right, Direction::Up, Direction::Down };
                auto it = std::remove(possibleDirections.begin(), possibleDirections.end(), oppositeDirection);
                possibleDirections.erase(it, possibleDirections.end());
                
                if (!possibleDirections.empty()) {
                    moveDirection = possibleDirections[std::rand() % possibleDirections.size()];
                }
            }
            else {
                moveDirection = nextDirection;
            }
        }
        else {
            // If path is empty, use the previous direction
            moveDirection = previousDirection;
        }
    }
    
    if(this->isReleased()){
        // Predict the future position based on the selected direction
        sf::Vector2f movement(0.f, 0.f);
        sf::Vector2f futurePosition = this->sprite.getPosition();
        switch (moveDirection) {
            case Direction::Left: futurePosition.x -= this->movementSpeed; movement.x = -this->movementSpeed; break;
            case Direction::Right: futurePosition.x += this->movementSpeed; movement.x = this->movementSpeed; break;
            case Direction::Up: futurePosition.y -= this->movementSpeed; movement.y = -this->movementSpeed; break;
            case Direction::Down: futurePosition.y += this->movementSpeed; movement.y = this->movementSpeed; break;
            case Direction::None: break;
        }

        sf::FloatRect futureBounds(futurePosition, ghostHead.getSize());
        bool collisionDetected = checkWallCollision(futureBounds, map);

        if (!collisionDetected && moveDirection != Direction::None && (modeChanged || this->isInGhostHouse(map) ||
                                                                       moveDirection != getOppositeDirection(previousDirection))) {
            // Move in the determined direction if no collision detected and not reverse (unless mode changed)
            this->sprite.move(movement);
            
            chosenDirection = moveDirection;
        }
        else {
            // Try alternative directions if primary direction is blocked or reverse
            std::vector<Direction> possibleDirections;
            
            std::vector<Direction> alternatives = { Direction::Left, Direction::Right, Direction::Up, Direction::Down };
            for (const auto& alternative : alternatives) {
                if (alternative == moveDirection || alternative == getOppositeDirection(previousDirection)) continue; // Skip invalid directions
                
                // Predict future position for the alternative direction
                sf::Vector2f futurePosition = this->sprite.getPosition();
                switch (alternative) {
                    case Direction::Left: futurePosition.x -= this->movementSpeed; break;
                    case Direction::Right: futurePosition.x += this->movementSpeed; break;
                    case Direction::Up: futurePosition.y -= this->movementSpeed; break;
                    case Direction::Down: futurePosition.y += this->movementSpeed; break;
                    case Direction::None: break;
                }

                sf::FloatRect futureBounds(futurePosition, ghostHead.getSize());
                bool collisionDetected = checkWallCollision(futureBounds, map);

                if (!collisionDetected) {
                    possibleDirections.push_back(alternative);
                }
            }

            // Choose a direction based on available options
            if (!possibleDirections.empty()) {
                // Randomly select a valid direction if available
                chosenDirection = possibleDirections[std::rand() % possibleDirections.size()];
                movement = sf::Vector2f(0.f, 0.f);
                switch (chosenDirection) {
                    case Direction::Left: movement.x = -this->movementSpeed; break;
                    case Direction::Right: movement.x = this->movementSpeed; break;
                    case Direction::Up: movement.y = -this->movementSpeed; break;
                    case Direction::Down: movement.y = this->movementSpeed; break;
                    case Direction::None: break;
                }
                this->sprite.move(movement);
            }
            else {
                // If no valid alternative directions are found, retain previous movement
                chosenDirection = previousDirection;
                movement = sf::Vector2f(0.f, 0.f);
                switch (chosenDirection) {
                    case Direction::Left: movement.x = -this->movementSpeed; break;
                    case Direction::Right: movement.x = this->movementSpeed; break;
                    case Direction::Up: movement.y = -this->movementSpeed; break;
                    case Direction::Down: movement.y = this->movementSpeed; break;
                    case Direction::None: break;
                }
                this->sprite.move(movement);
            }
        }

        // Update previous direction
        previousDirection = chosenDirection;
        movementDirection = chosenDirection;
    }
    
    if (isFrightened) {
        updateFrightenedAnimation();
    } else if (returnToGhostHouse) {
        updateReturnToHomeAnimation();
    } else {
        updateNormalAnimation();
    }
}


void Ghost::updateFrightenedAnimation() {
    this->frameTime += this->animationSpeed;
    float elapsedTime = frightenedClock.getElapsedTime().asSeconds();

    // Check if it's time to start flashing (after 6 seconds)
    if (elapsedTime >= 6.0f) {
        // Alternate between blue and white frames
        bool useWhiteFrames = static_cast<int>((elapsedTime - 6.0f) * 10) % 2 == 0;

        if (useWhiteFrames) {
            // White frightened frames
            int frameX = 390 + (this->currentFrame * 17);
            this->sprite.setTextureRect(sf::IntRect(frameX, 601, 14, 14));
        } else {
            // Blue frightened frames
            int frameX = 390 + (this->currentFrame * 17);
            this->sprite.setTextureRect(sf::IntRect(frameX, 567, 14, 14));
        }
    } else {
        // Regular blue frightened frames
        int frameX = 390 + (this->currentFrame * 17);
        this->sprite.setTextureRect(sf::IntRect(frameX, 567, 14, 14));
    }

    // Update frame time for animation
    if (this->frameTime >= 1.f) {
        this->frameTime = 0.f;
        this->currentFrame = (this->currentFrame + 1) % 2; // Cycle between the two frames
    }
}


void Ghost::updateReturnToHomeAnimation() {
    int frameX = 0;
    int frameY = 584; // Row for return-to-home animation

    switch (this->movementDirection) {
        case Direction::Left:
            frameX = 390; // Left direction frame
            break;
        case Direction::Up:
            frameX = 407; // Up direction frame
            break;
        case Direction::Right:
            frameX = 424; // Right direction frame
            break;
        case Direction::Down:
            frameX = 441; // Down direction frame
            break;
        default:
            frameX = 424;
            break;
    }
    this->sprite.setTextureRect(sf::IntRect(frameX, frameY, 14, 14));
}


void Ghost::updateNormalAnimation() {
    this->frameTime += this->animationSpeed;
    if (this->frameTime >= 1.f) {
        this->frameTime = 0.f;
        this->currentFrame = (this->currentFrame + 1) % 2; // Cycle between the two frames

        int frameX = 169;
        int frameY = this->spriteYOffset;

        switch (this->movementDirection) {
            case Direction::Right:
                frameX = 169 + (this->currentFrame * 17);
                break;
            case Direction::Left:
                frameX = 101 + (this->currentFrame * 17);
                break;
            case Direction::Up:
                frameX = 135 + (this->currentFrame * 17);
                break;
            case Direction::Down:
                frameX = 203 + (this->currentFrame * 17);
                break;
            default:
                frameX = 169;
                break;
        }

        this->sprite.setTextureRect(sf::IntRect(frameX, frameY, 14, 14));
    }
}
