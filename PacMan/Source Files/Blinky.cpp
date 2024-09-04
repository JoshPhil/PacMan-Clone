#include "Blinky.hpp"


Blinky::Blinky(float x, float y, int spriteYOffset, int tileSize) : Ghost(x, y, spriteYOffset, tileSize) {}

Blinky::~Blinky() {}

sf::Vector2i Blinky::getTargetTile(const sf::Vector2f& pacManPosition, Direction pacManDirection, const sf::Vector2f& blinkyPosition, const std::vector<std::string>& map) {
    return sf::Vector2i(static_cast<int>(pacManPosition.x / 28), static_cast<int>(pacManPosition.y / 28));
}


std::vector<sf::Vector2i> Blinky::getCornerLoop() const {
    return {
        {18, 1}, {17, 1}, {16, 1}, {15, 1}, {15, 2}, {15, 3}, {16, 3}, {17, 3}, {18, 3}, {18, 2}
    };
}
