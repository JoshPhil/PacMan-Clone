#include "Inky.hpp"
#include <cmath> // For std::sqrt and std::pow

Inky::Inky(float x, float y, int spriteYOffset, int tileSize) : Ghost(x, y, spriteYOffset, tileSize) {}

Inky::~Inky() {}

sf::Vector2i Inky::getTargetTile(const sf::Vector2f& pacManPosition, Direction pacManDirection, const sf::Vector2f& blinkyPosition, const std::vector<std::string>& map) {
    sf::Vector2i targetTile;
    sf::Vector2i initialTargetTile;

    // Calculate initial target tile position based on Pac-Man's position and direction
    switch (pacManDirection) {
        case Direction::Left:
            initialTargetTile = sf::Vector2i(static_cast<int>(pacManPosition.x / tileSize) - 2, static_cast<int>(pacManPosition.y / tileSize));
            break;
        case Direction::Right:
            initialTargetTile = sf::Vector2i(static_cast<int>(pacManPosition.x / tileSize) + 2, static_cast<int>(pacManPosition.y / tileSize));
            break;
        case Direction::Up:
            initialTargetTile = sf::Vector2i(static_cast<int>(pacManPosition.x / tileSize), static_cast<int>(pacManPosition.y / tileSize) - 2);
            break;
        case Direction::Down:
            initialTargetTile = sf::Vector2i(static_cast<int>(pacManPosition.x / tileSize), static_cast<int>(pacManPosition.y / tileSize) + 2);
            break;
        default:
            initialTargetTile = sf::Vector2i(static_cast<int>(pacManPosition.x / tileSize), static_cast<int>(pacManPosition.y / tileSize));
            break;
    }

    // Calculate the vector from Blinky to the initial target tile
    sf::Vector2i blinkyTile(static_cast<int>(blinkyPosition.x / tileSize), static_cast<int>(blinkyPosition.y / tileSize));
    sf::Vector2i vectorToInitialTarget = initialTargetTile - blinkyTile;

    // Double the length of the vector
    sf::Vector2i extendedVector = blinkyTile + vectorToInitialTarget * 2;

    targetTile = extendedVector;
    
    // Ensure targetTile is within bounds and valid
    if (isTileOutOfBounds(targetTile, map) || checkWallCollision(sf::FloatRect(targetTile.x * tileSize, targetTile.y * tileSize, tileSize, tileSize), map)) {
        targetTile = findClosestValidTile(targetTile, map);
    }

    return targetTile;
}

sf::Vector2i Inky::findClosestValidTile(const sf::Vector2i& startingTile, const std::vector<std::string>& map) const {
    const int maxDistance = 2; // Maximum distance to check in each direction
    std::vector<sf::Vector2i> directions = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };
    sf::Vector2i closestTile = startingTile;
    float closestDistance = std::numeric_limits<float>::max();

    for (int d = 1; d <= maxDistance; ++d) {
        for (const auto& direction : directions) {
            sf::Vector2i candidateTile = startingTile + direction * d;

            // Create a bounding rectangle for the candidate tile
            sf::FloatRect candidateTileBounds(candidateTile.x * 28, candidateTile.y * 28, 28, 28);

            if (!isTileOutOfBounds(candidateTile, map) && !checkWallCollision(candidateTileBounds, map)) {
                float distance = std::sqrt(static_cast<float>(std::pow(candidateTile.x - startingTile.x, 2) +
                                                               std::pow(candidateTile.y - startingTile.y, 2)));
                if (distance < closestDistance) {
                    closestDistance = distance;
                    closestTile = candidateTile;
                }
            }
        }
    }

    return closestTile;
}

std::vector<sf::Vector2i> Inky::getCornerLoop() const {
    return {
        {18, 19}, {17, 19}, {16, 19}, {15, 19}, {14, 19}, {13, 19}, {12, 19}, {11, 19}, {11, 18}, {11, 17}, {12, 17}, {13, 17}
    };
}

bool Inky::isTileOutOfBounds(const sf::Vector2i& tile, const std::vector<std::string>& map) const {
    return (tile.y < 0 || tile.y >= static_cast<int>(map.size()) ||
            tile.x < 0 || tile.x >= static_cast<int>(map[tile.y].size()) ||
            map[tile.y][tile.x] == '0');
}
