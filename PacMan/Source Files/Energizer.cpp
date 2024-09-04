#include "Energizer.hpp"

void Energizer::initVariables() {
    this->eaten = false;
}

void Energizer::initShape() {
    this->shape.setFillColor(sf::Color(255, 229, 180)); // Peach
    this->shape.setRadius(7.f);
}

Energizer::Energizer(float x, float y, float radius) {
    this->shape.setPosition(x, y);
    this->shape.setRadius(radius);
    this->initVariables();
    this->initShape();
}

Energizer::~Energizer() {}

bool Energizer::isEaten() const {
    return this->eaten;
}

void Energizer::setEaten(bool state) {
    this->eaten = state;
}

void Energizer::update() {
    // Update logic for the pellet, if any
}

void Energizer::render(sf::RenderTarget* target) {
    if (!this->eaten) {
        target->draw(this->shape);
    }
}

const sf::CircleShape& Energizer::getShape() const {
    return this->shape;
}
