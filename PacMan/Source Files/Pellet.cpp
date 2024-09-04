#include "Pellet.hpp"

extern int globalSoundState;

void Pellet::initVariables() {
    this->eaten = false;
}

void Pellet::initShape() {
    this->shape.setFillColor(sf::Color(255, 229, 180)); // Peach
    this->shape.setRadius(3.f);
}

void Pellet::initSounds() {
    if (!chopBuffer0.loadFromFile("Sound Effects/eat_dot_0.wav")) {
        // Handle error
    }
    if (!chopBuffer1.loadFromFile("Sound Effects/eat_dot_1.wav")) {
        // Handle error
    }
}

Pellet::Pellet(float x, float y, float radius) {
    this->shape.setPosition(x, y);
    this->shape.setRadius(radius);
    this->initVariables();
    this->initShape();
    this->initSounds();
}

Pellet::~Pellet() {}

bool Pellet::isEaten() const {
    return this->eaten;
}

void Pellet::setEaten(bool state) {
    if (globalSoundState == 0) {
        this->chopSound.setBuffer(chopBuffer0);
        globalSoundState = 1;
    } else {
        this->chopSound.setBuffer(chopBuffer1);
        globalSoundState = 0;
    }
    this->chopSound.play();
    this->eaten = state;
}

void Pellet::update() {
    // Update logic for the pellet, if any
}

void Pellet::render(sf::RenderTarget* target) {
    if (!this->eaten) {
        target->draw(this->shape);
    }
}

const sf::CircleShape& Pellet::getShape() const {
    return this->shape;
}
