#pragma once

#include "Ghost.hpp"

class Clyde : public Ghost {
private:
    sf::Vector2i getTargetTile(const sf::Vector2f& pacManPosition, Direction pacManDirection, const sf::Vector2f& blinkyPosition, const std::vector<std::string>& map) override;
    std::vector<sf::Vector2i> getCornerLoop() const override;
    sf::Vector2i findClosestValidTile(const sf::Vector2i& startingTile, const std::vector<std::string>& map) const;
    bool isTileOutOfBounds(const sf::Vector2i& tile, const std::vector<std::string>& map) const;

public:
    Clyde(float x, float y, int spriteYOffset, int tileSize);
    ~Clyde();
};


