#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>

class TextureManager {
private:
    std::unordered_map<std::string, sf::Texture> textures;

    // Private constructor for singleton
    TextureManager() = default;

public:
    // Get the single instance of TextureManager
    static TextureManager& getInstance();

    const sf::Texture& getTexture(const std::string& filePath);

    bool loadTexture(const std::string& filePath);
};
