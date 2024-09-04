#include "TextureManager.hpp"
#include <iostream>

TextureManager& TextureManager::getInstance() {
    static TextureManager instance;  // Guaranteed to be destroyed and instantiated on first use
    return instance;
}

const sf::Texture& TextureManager::getTexture(const std::string& filePath) {
    if (textures.find(filePath) != textures.end()) {
        return textures[filePath];
    }

    sf::Texture texture;
    if (!texture.loadFromFile(filePath)) {
        std::cerr << "Error loading texture from file: " << filePath << std::endl;
    }

    textures[filePath] = texture;
    return textures[filePath];
}

bool TextureManager::loadTexture(const std::string& filePath) {
    sf::Texture texture;
    if (!texture.loadFromFile(filePath)) {
        std::cerr << "Error loading texture from file: " << filePath << std::endl;
        return false;
    }

    textures[filePath] = texture;
    return true;
}
