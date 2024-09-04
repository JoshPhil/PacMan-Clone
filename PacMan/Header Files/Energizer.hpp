#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

class Energizer {
private:
    sf::CircleShape shape;
    bool eaten;

    void initVariables();
    void initShape();

public:
    Energizer(float x, float y, float radius = 7.f);
    virtual ~Energizer();

    bool isEaten() const;
    void setEaten(bool state);

    void update();
    void render(sf::RenderTarget* target);

    const sf::CircleShape& getShape() const;
};
