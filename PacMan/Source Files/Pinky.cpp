#include "Pinky.hpp"
#include <cmath> // For std::sqrt and std::pow

Pinky::Pinky(float x, float y, int spriteYOffset, int tileSize) : Ghost(x, y, spriteYOffset, tileSize) {}

Pinky::~Pinky() {}

sf::Vector2i Pinky::getTargetTile(const sf::Vector2f& pacManPosition, Direction pacManDirection, const sf::Vector2f& blinkyPosition, const std::vector<std::string>& map) {
    sf::Vector2i targetTile;

    switch (pacManDirection) {
        case Direction::Left:
            targetTile = sf::Vector2i(static_cast<int>(pacManPosition.x / tileSize) - 4, static_cast<int>(pacManPosition.y / tileSize));
            break;
        case Direction::Right:
            targetTile = sf::Vector2i(static_cast<int>(pacManPosition.x / tileSize) + 4, static_cast<int>(pacManPosition.y / tileSize));
            break;
        case Direction::Up:
            targetTile = sf::Vector2i(static_cast<int>(pacManPosition.x / tileSize), static_cast<int>(pacManPosition.y / tileSize) - 4);
            break;
        case Direction::Down:
            targetTile = sf::Vector2i(static_cast<int>(pacManPosition.x / tileSize), static_cast<int>(pacManPosition.y / tileSize) + 4);
            break;
        default:
            targetTile = sf::Vector2i(static_cast<int>(pacManPosition.x / tileSize), static_cast<int>(pacManPosition.y / tileSize));
            break;
    }

    // Ensure targetTile is within bounds and valid
    if (isTileOutOfBounds(targetTile, map) || checkWallCollision(sf::FloatRect(targetTile.x * tileSize, targetTile.y * tileSize, tileSize, tileSize), map)) {
        targetTile = findClosestValidTile(targetTile, map);
    }

    return targetTile;
}

sf::Vector2i Pinky::findClosestValidTile(const sf::Vector2i& startingTile, const std::vector<std::string>& map) const {
    const int maxDistance = 4; // Maximum distance to check in each direction
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

std::vector<sf::Vector2i> Pinky::getCornerLoop() const {
    return {
        {2, 1}, {3, 1}, {4, 1}, {5, 1}, {5, 2}, {5, 3}, {4, 3}, {3, 3}, {2, 3}, {2, 2}
    };
}

bool Pinky::isTileOutOfBounds(const sf::Vector2i& tile, const std::vector<std::string>& map) const {
    return (tile.y < 0 || tile.y >= static_cast<int>(map.size()) ||
            tile.x < 0 || tile.x >= static_cast<int>(map[tile.y].size()) ||
            map[tile.y][tile.x] == '0');
}
