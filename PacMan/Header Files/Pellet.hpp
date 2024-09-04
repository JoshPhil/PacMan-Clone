#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

class Pellet {
private:
    sf::CircleShape shape;
    bool eaten;

    void initVariables();
    void initShape();
    void initSounds();
    
    sf::SoundBuffer chopBuffer0;
    sf::SoundBuffer chopBuffer1;
    sf::Sound chopSound;

public:
    Pellet(float x, float y, float radius = 3.f);
    virtual ~Pellet();

    bool isEaten() const;
    void setEaten(bool state);
    
    void update();
    void render(sf::RenderTarget* target);

    const sf::CircleShape& getShape() const;
};
