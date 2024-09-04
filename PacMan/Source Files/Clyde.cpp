#include "Clyde.hpp"
#include <cmath> // For std::abs

Clyde::Clyde(float x, float y, int spriteYOffset, int tileSize) : Ghost(x, y, spriteYOffset, tileSize) {}

Clyde::~Clyde() {}

sf::Vector2i Clyde::getTargetTile(const sf::Vector2f& pacManPosition, Direction pacManDirection, const sf::Vector2f& blinkyPosition, const std::vector<std::string>& map) {
    sf::Vector2i targetTile;
    sf::Vector2i clydeTile(static_cast<int>(getPosition().x / tileSize), static_cast<int>(getPosition().y / tileSize));
    sf::Vector2i pacManTile(static_cast<int>(pacManPosition.x / tileSize), static_cast<int>(pacManPosition.y / tileSize));

    // Calculate the Manhattan distance between Clyde and Pac-Man
    int distance = std::abs(pacManTile.x - clydeTile.x) + std::abs(pacManTile.y - clydeTile.y);

    // Determine target based on distance
    if (distance > 8) {
        // Clyde targets Pac-Man's current tile
        targetTile = pacManTile;
    } else {
        // Clyde targets a fixed tile in Scatter mode
        targetTile = {3, 19}; // Bottom-left corner of the maze
    }

    // Ensure targetTile is within bounds and valid
    if (isTileOutOfBounds(targetTile, map) || checkWallCollision(sf::FloatRect(targetTile.x * tileSize, targetTile.y * tileSize, tileSize, tileSize), map)) {
        targetTile = findClosestValidTile(targetTile, map);
    }

    return targetTile;
}

sf::Vector2i Clyde::findClosestValidTile(const sf::Vector2i& startingTile, const std::vector<std::string>& map) const {
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

std::vector<sf::Vector2i> Clyde::getCornerLoop() const {
    return {
        {3, 19}, {4, 19}, {5, 19}, {6, 19}, {7, 19}, {8, 19}, {9, 19},
        {9, 18}, {9, 17}, {8, 17}, {7, 17}, {7, 16}, {7, 15}, {6, 15},
        {5, 15}, {5, 16}, {5, 17}, {4, 17}, {3, 17}, {2, 17}, {2, 18}, {2, 19}
    };
}

bool Clyde::isTileOutOfBounds(const sf::Vector2i& tile, const std::vector<std::string>& map) const {
    return (tile.y < 0 || tile.y >= static_cast<int>(map.size()) ||
            tile.x < 0 || tile.x >= static_cast<int>(map[tile.y].size()) ||
            map[tile.y][tile.x] == '0');
}
