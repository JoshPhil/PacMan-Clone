#pragma once

#include "Ghost.hpp"

class Blinky : public Ghost {
private:
    sf::Vector2i getTargetTile(const sf::Vector2f& pacManPosition, Direction pacManDirection, const sf::Vector2f& blinkyPosition, const std::vector<std::string>& map) override;
    std::vector<sf::Vector2i> getCornerLoop() const override;

public:
    Blinky(float x, float y, int spriteYOffset, int tileSize);
    ~Blinky();
};

