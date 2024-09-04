#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <vector>

#include "PacMan.hpp"
#include "Pellet.hpp"
#include "Ghost.hpp"
#include "Energizer.hpp"

class Game{
private:
    
    sf::Vector2u baseResolution;
    
    float scaleX;
    float scaleY;
    
    sf::Clock endGameClock;
    
    sf::Clock pauseClock;
    bool isPaused;
    
    bool isRestarting;
    sf::Clock restartClock;
    
    bool isGamePaused;
    
    sf::Clock flashClock;
    int flashCount;
    bool isFlashing;
    
    int ghostEatenCount;
    
    sf::VideoMode videoMode;
    std::unique_ptr<sf::RenderWindow> window;
    bool endGame;
    sf::Event sfmlEvent;
    
    sf::Music introMusic;
    bool isMusicPlaying;
    
    sf::SoundBuffer sirenBuffer;
    sf::Sound sirenSound;
    bool isSirenPlaying;
    
    sf::SoundBuffer frightenedBuffer;
    sf::Sound frightenedSound;
    bool isFrightenedSoundPlaying;
    
    sf::SoundBuffer eyesBuffer;
    sf::Sound eyesSound;
    bool isEyesSoundPlaying;
    
    sf::SoundBuffer deathBuffer;
    sf::Sound deathSound;
    bool isDeathSoundPlaying;
    
    sf::SoundBuffer eatGhostBuffer;
    sf::Sound eatGhostSound;
    bool isEatGhostSoundPlaying;
    
    PacMan pacman;
    std::vector<Pellet> pellets;
    std::vector<Energizer> energizers;
    int pelletsEaten;
    std::vector<std::unique_ptr<Ghost>> ghosts;
    std::vector<std::string> map;
    std::vector<std::string> originalMap;
    int tileSize;
    
    int scatterCounter;
    int chaseCounter;
    bool modeChanged;
    
    void updateModeCounters();
    void initVariables();
    void initWindow();
    void initPacMan();
    void initPellets();
    void initGhosts();
    void updateGhostsRelease();
    void renderMap();
    void initSounds();
    void flashWalls();
    void restartGame();
    
public:
    Game();
    virtual ~Game();
    
    const bool running() const;
    
    bool getEndGame() const;
    void setEndGame(bool state);
    
    void resetGhostEatenCount();
    
    int getTileSize() const;
    
    int getChaseCounter() const;
    int getScatterCounter() const;
    void pollEvents();
    void update();
    void render();
};
