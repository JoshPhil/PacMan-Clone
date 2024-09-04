#include "Game.hpp"
#include "PacMan.hpp"
#include "Pellet.hpp"
#include "Blinky.hpp"
#include "Pinky.hpp"
#include "Inky.hpp"
#include "Clyde.hpp"

int globalSoundState = 0;

Game::Game(){
    this->initWindow();
    this->map = {
            "0###################0",
            "0#........#........#0",
            "0#o##.###.#.###.##o#0",
            "0#.................#0",
            "0#.##.#.#####.#.##.#0",
            "0#....#...#...#....#0",
            "0####.### # ###.####0",
            "0000#.#   1   #.#0000",
            "#####.# ##-## #.#####",
            "     .  #234#  .     ",
            "#####.# ##### #.#####",
            "0000#.#       #.#0000",
            "0####.# ##### #.####0",
            "0#........#........#0",
            "0#.##.###.#.###.##.#0",
            "0#o.#.....P.....#.o#0",
            "0##.#.#.#####.#.#.##0",
            "0#....#...#...#....#0",
            "0#.######.#.######.#0",
            "0#.................#0",
            "0###################0"
        };
    this->initVariables();
    this->originalMap = this->map;
    this->initPacMan();
    this->initPellets();
    this->initGhosts();
    
    this->initSounds();
    
    // Load and play the intro music
    if (!this->introMusic.openFromFile("Sound Effects/start.wav")) {
        // Handle error
    }
    this->introMusic.play();
    this->isMusicPlaying = true;
}


Game::~Game() = default;


void Game::initVariables() {
    this->tileSize = 28;
    this->endGame = false;
    this->modeChanged = false;
    this->scatterCounter = 0;
    this->chaseCounter = 0;
    this->isGamePaused = false;
    this->ghostEatenCount = 0;
    this->flashClock.restart();
    this->flashCount = 0;
    this->isFlashing = false;
    this->pelletsEaten = 0;
    this->isPaused = true;
    this->isRestarting = false;

}


void Game::initWindow(){
    this->videoMode = sf::VideoMode(588, 588);
    this->window = std::make_unique<sf::RenderWindow>(this->videoMode, "PacMan", sf::Style::Close | sf::Style::Titlebar);
    this->window->setFramerateLimit(60);
}


void Game::renderMap() {
    // Calculate the offsets to center the map in the window
    float offsetX = (this->videoMode.width - map[0].size() * this->tileSize) / 2.f;
    float offsetY = (this->videoMode.height - map.size() * this->tileSize) / 2.f;

    for (size_t y = 0; y < map.size(); ++y){
        for (size_t x = 0; x < map[y].size(); ++x){
            char cell = map[y][x];
            sf::RectangleShape tile(sf::Vector2f(this->tileSize, this->tileSize));
            
            if (cell == '#') {
                tile.setFillColor(sf::Color::Blue); // Default wall color
            }
            else if (cell == 'w') {
                tile.setFillColor(sf::Color::White); // Flashing wall color
            }
            else if (cell == '-') {
                tile.setFillColor(sf::Color(190, 225, 255)); //  Ghost house door
            }
            else if (cell == '.' || cell == 'o' || cell == ' '|| cell == '0'  ) {
                tile.setFillColor(sf::Color::Black); // Pathway
            }
            
            tile.setPosition(x * this->tileSize + offsetX, y * this->tileSize + offsetY);
            this->window->draw(tile);
        }
    }
}


void Game::initPacMan() {
    float pacManX = 0.f;
    float pacManY = 0.f;

    // Locate the 'P' in the map and set PacMan's position
    for (size_t y = 0; y < map.size(); ++y) {
        for (size_t x = 0; x < map[y].size(); ++x) {
            if (map[y][x] == 'P') {
                pacManX = x * this->tileSize + this->tileSize / 2 - 13.1; // Adjust for PacMan's center
                pacManY = y * this->tileSize + this->tileSize / 2 - 13.1; // Adjust for PacMan's center
                map[y][x] = ' '; // Remove 'P' from the map
                break;
            }
        }
    }

    this->pacman = PacMan(pacManX, pacManY);
}


void Game::initGhosts() {
    for (size_t y = 0; y < map.size(); ++y) {
        for (size_t x = 0; x < map[y].size(); ++x) {
            char cell = map[y][x];
            float ghostX = x * this->tileSize + this->tileSize / 2 - 13.1; // Centered
            float ghostY = y * this->tileSize + this->tileSize / 2 - 13.1; // Centered

            if (cell == '1') {
                ghosts.emplace_back(std::make_unique<Blinky>(ghostX, ghostY, 567, this->tileSize)); // Blinky, y offset = 567
                map[y][x] = ' '; // Clear the ghost position in the map
            }
            else if (cell == '2') {
                ghosts.emplace_back(std::make_unique<Pinky>(ghostX, ghostY, 584, this->tileSize)); // Pinky, y offset = 584
                map[y][x] = ' '; // Clear the ghost position in the map
            }
            else if (cell == '3') {
                ghosts.emplace_back(std::make_unique<Inky>(ghostX, ghostY, 601, this->tileSize)); // Inky, y offset = 601
                map[y][x] = ' '; // Clear the ghost position in the map
            }
            else if (cell == '4') {
                ghosts.emplace_back(std::make_unique<Clyde>(ghostX, ghostY, 618, this->tileSize)); // Clyde, y offset = 618
                map[y][x] = ' '; // Clear the ghost position in the map
            }
        }
    }
}


void Game::initPellets() {
    for (size_t y = 0; y < map.size(); ++y) {
        for (size_t x = 0; x < map[y].size(); ++x) {
            if (map[y][x] == '.') {
                float posX = x * this->tileSize + this->tileSize / 2 - 3; // Adjust for pellet center
                float posY = y * this->tileSize + this->tileSize / 2 - 3; // Adjust for pellet center
                pellets.emplace_back(posX, posY);
            }
            else if(map[y][x] == 'o'){
                float posX = x * this->tileSize + this->tileSize / 2 - 7; // Adjust for pellet center
                float posY = y * this->tileSize + this->tileSize / 2 - 7; // Adjust for pellet center
                energizers.emplace_back(posX, posY);
            }
        }
    }
}


void Game::initSounds() {
    if (!this->sirenBuffer.loadFromFile("Sound Effects/siren0.wav")) {
        // Handle error
    }
    this->sirenSound.setBuffer(this->sirenBuffer);
    this->sirenSound.setLoop(true);
    this->isSirenPlaying = false;
    
    if (!this->frightenedBuffer.loadFromFile("Sound Effects/fright_firstloop.wav")) {
        // Handle error
    }
    this->frightenedSound.setBuffer(this->frightenedBuffer);
    this->frightenedSound.setLoop(true);
    this->isFrightenedSoundPlaying = false;
    
    if (!this->eyesBuffer.loadFromFile("Sound Effects/eyes.wav")) {
        // Handle error
    }
    this->eyesSound.setBuffer(this->eyesBuffer);
    this->eyesSound.setLoop(true);
    this->isEyesSoundPlaying = false;
    
    if (!this->deathBuffer.loadFromFile("Sound Effects/death.wav")) {
        // Handle error
    }
    this->deathSound.setBuffer(this->deathBuffer);
    this->isDeathSoundPlaying = false;
    
    if (!this->eatGhostBuffer.loadFromFile("Sound Effects/eat_ghost.wav")) {
        // Handle error
    }
    this->eatGhostSound.setBuffer(this->eatGhostBuffer);
    this->isEatGhostSoundPlaying = false;
}


bool Game::getEndGame() const{
    return this->endGame;
}


int Game::getTileSize() const{
    return this->tileSize;
}


void Game::setEndGame(bool state) {
    endGame = state;
}


void Game::resetGhostEatenCount() {
    ghostEatenCount = 0;
}


const bool Game::running() const{
    return this->window->isOpen();
}


void Game::pollEvents(){
    while(this->window->pollEvent(this->sfmlEvent)){
        if (this->sfmlEvent.type == sf::Event::Closed){
            this->window->close();
        }
        else if (this->sfmlEvent.type == sf::Event::KeyPressed){
            if (this->sfmlEvent.key.code == sf::Keyboard::Escape){
                this->window->close();
            }
            this->pacman.handleKeyPress(this->sfmlEvent.key.code);
        }
    }
}


void Game::updateGhostsRelease() {
    for (auto& ghost : ghosts) {
        if (auto blinky = dynamic_cast<Blinky*>(ghost.get())) {
            if (!blinky->isReleased()) blinky->release();
        } else if (auto pinky = dynamic_cast<Pinky*>(ghost.get())) {
            if (pinky->getIsGhostInHouse() && !pinky->isReleased()) pinky->release();
        } else if (auto inky = dynamic_cast<Inky*>(ghost.get())) {
            if (inky->getIsGhostInHouse() && !inky->isReleased() && pelletsEaten >= 30) inky->release();
        } else if (auto clyde = dynamic_cast<Clyde*>(ghost.get())) {
            if (clyde->getIsGhostInHouse() && !clyde->isReleased() && pelletsEaten >= (pellets.size() / 3)) clyde->release();
        }
    }
}


void Game::updateModeCounters() {
    modeChanged = false;
    
    // Increment counters based on current mode
    if (ghosts[0]->getIsChaseMode()) {
        chaseCounter++;
        if (chaseCounter > 20 * 60) {
            // Handle mode change
            modeChanged = true;
            chaseCounter = 0;
        }
    }
    else {
        scatterCounter++;
        if (scatterCounter > 7 * 60) {
            // Handle mode change
            modeChanged = true;
            scatterCounter = 0;
        }
    }
}


int Game::getChaseCounter() const {
    // Assuming you store the counters in a member variable
    return chaseCounter;
}


int Game::getScatterCounter() const {
    return scatterCounter;
}


void Game::restartGame() {
    this->map = this->originalMap;
    // Reinitialize variables and objects
    this->ghosts.clear();
    this->pellets.clear();
    this->energizers.clear();
    this->pacman = PacMan();
    this->initVariables();
    this->initPacMan();
    this->initGhosts();
    this->initPellets();
    
    isRestarting = true;
    restartClock.restart();
}


void Game::flashWalls() {
    // Change the wall color every 0.2 seconds
    if (flashClock.getElapsedTime().asSeconds() >= 0.5f) {
        flashClock.restart();
        flashCount++;

        // Toggle wall color between white and original color
        for (size_t y = 0; y < map.size(); ++y) {
            for (size_t x = 0; x < map[y].size(); ++x) {
                if (originalMap[y][x] == '#') {
                    map[y][x] = (flashCount % 2 == 0) ? '#' : 'w';
                }
            }
        }

        // Check if flashing is complete
        if (flashCount >= 6) { // Flash 3 times (each flash consists of 2 color changes)
            isFlashing = false;
            restartGame();
            return;
        }
    }
}


void Game::update() {
    if (this->endGame) {
        // Stop all ongoing sounds
        this->sirenSound.stop();
        this->frightenedSound.stop();
        this->eyesSound.stop();

        // Reset sound status flags
        this->isSirenPlaying = false;
        this->isFrightenedSoundPlaying = false;
        this->isEyesSoundPlaying = false;

        // Check if 1 second has passed since the game ended
        if (endGameClock.getElapsedTime().asSeconds() < 1.f) {
            return; // Wait for 1 second before proceeding
        } else {
            // Play the death sound after 1 second delay
            if (!this->isDeathSoundPlaying) {
                this->deathSound.play();
                this->isDeathSoundPlaying = true;
                return;
            }
            
            // Check if the death sound has finished playing
            if (this->deathSound.getStatus() == sf::Sound::Stopped) {
                this->isDeathSoundPlaying = false;
                this->restartGame(); // Restart the game after the sound finishes
                return;
            }
        }
        
    } else {
        // If the game is not over, reset the clock
        endGameClock.restart();
    }
    
    if (isRestarting) {
        if (restartClock.getElapsedTime().asSeconds() >= 2.f) {
            isRestarting = false; // End the pause
        }
        return; // Skip other updates while paused
    }
    
    
    // Check if all pellets and energizers are eaten
    if (pelletsEaten >= (pellets.size() + energizers.size())) {
        isFlashing = true;
        this->sirenSound.stop();
        this->frightenedSound.stop();
        this->eyesSound.stop();

        // Reset sound status flags
        this->isSirenPlaying = false;
        this->isFrightenedSoundPlaying = false;
        this->isEyesSoundPlaying = false;
    }

    if (pelletsEaten >= (pellets.size()) + 4) {
        this->sirenSound.stop();
        this->frightenedSound.stop();
        this->eyesSound.stop();

        // Reset sound status flags
        this->isSirenPlaying = false;
        this->isFrightenedSoundPlaying = false;
        this->isEyesSoundPlaying = false;
        if (isPaused) {
            // Check if 3 seconds have passed
            if (pauseClock.getElapsedTime().asSeconds() >= 1.5f) {
                isFlashing = true;
                isPaused = false; // End the pause
            }
            return; // Skip other updates while paused
        }
    }
    else{
        pauseClock.restart();
    }
    
    
    // Handle wall flashing
    if (isFlashing) {
        flashWalls();
        return; // Skip other updates while flashing
    }
    
    
    this->pollEvents();
    if (this->isMusicPlaying) {
        if (this->introMusic.getStatus() != sf::Music::Playing) {
            this->isMusicPlaying = false;
        } else {
            return;
        }
    }
    
    if (!this->isEatGhostSoundPlaying) {
        this->pacman.update(this->window.get(), this->map, this->pellets, this->energizers, this->ghosts, this);
    }
    
    // Check if any ghost has been eaten and play the sound
    Ghost* eatenGhost = nullptr;
    for (auto& ghost : ghosts) {
        if (ghost->isEaten()) {
            eatenGhost = ghost.get();
            break; // We only need to track the first eaten ghost
        }
    }

    // Play the eat ghost sound if it hasn't been played yet
    if (eatenGhost && !this->isEatGhostSoundPlaying) {
        this->eatGhostSound.play();
        this->isEatGhostSoundPlaying = true;
        this->ghostEatenCount++;
    }

    // Check if the sound has finished playing
    if (this->isEatGhostSoundPlaying && this->eatGhostSound.getStatus() == sf::Sound::Stopped) {
        this->isEatGhostSoundPlaying = false;
        if (eatenGhost) {
            eatenGhost->setEaten(false);
        }
    }

    if (!this->endGame) {
        int newPelletsEaten = 0;
        
        for (const auto& pellet : pellets) {
            if (pellet.isEaten()) {
                newPelletsEaten++;
            }
        }
        
        for (const auto& energizer : energizers) {
            if (energizer.isEaten()) {
                newPelletsEaten++;
            }
        }
        
        if (newPelletsEaten != pelletsEaten) {
            pelletsEaten = newPelletsEaten;
            updateGhostsRelease();
        }

        updateModeCounters();
        
        bool allGhostsInNormalMode = true;
        bool anyGhostReturningToHouse = false;
        for (auto& ghost : ghosts) {
            if (ghost->getIsFrightened()) {
                allGhostsInNormalMode = false;
            }
            if (ghost->getIsReturningToHouse()) {
                anyGhostReturningToHouse = true;
            }
        }
        
        if (allGhostsInNormalMode) {
            this->ghostEatenCount = 0;
        }

        if (allGhostsInNormalMode && !this->isSirenPlaying) {
            // Stop other sounds and play siren
            this->eyesSound.stop();
            this->isEyesSoundPlaying = false;
            this->frightenedSound.stop();
            this->isFrightenedSoundPlaying = false;
            
            this->sirenSound.play();
            this->isSirenPlaying = true;
        } else if (!allGhostsInNormalMode && this->isSirenPlaying) {
            // Stop siren and play frightened sound
            this->sirenSound.stop();
            this->isSirenPlaying = false;
            
            this->frightenedSound.play();
            this->isFrightenedSoundPlaying = true;
        }

        
        if (anyGhostReturningToHouse && !this->isEyesSoundPlaying) {
            this->eyesSound.play();
            this->isEyesSoundPlaying = true;
        } else if (!anyGhostReturningToHouse && this->isEyesSoundPlaying) {
            this->eyesSound.stop();
            this->isEyesSoundPlaying = false;
        }


        if (modeChanged) {
            for (auto& ghost : ghosts) {
                ghost->switchMode();
            }
        }
        
    
        // Update ghosts
        for (auto& ghost : ghosts) {
            if(this->isEatGhostSoundPlaying && !ghost->isEaten()){
                continue;
            }
            sf::Vector2f blinkyPosition = static_cast<Blinky*>(ghost.get())->getPosition();
            ghost->update(this->pacman.getPosition(), this->pacman.getCurrentDirection(), blinkyPosition, this->map, static_cast<int>(this->pellets.size()), this->window.get(), modeChanged, this->getChaseCounter(), this->getScatterCounter(), this->ghostEatenCount);
        }
    }
}


void Game::render() {
    this->window->clear();
    this->renderMap();
    
    // Render pellets
    for (auto& pellet : pellets) {
        pellet.render(this->window.get());
    }
    
    // Render energizers
    for (auto& energizer : energizers) {
        energizer.render(this->window.get());
    }
    
    if (!this->endGame || endGameClock.getElapsedTime().asSeconds() < 1.f) {
        for (auto& ghost : ghosts) {
            ghost->render(this->window.get());
        }
    }
    
    if (!this->isEatGhostSoundPlaying) {
        this->pacman.render(this->window.get());
    }
    this->window->display();
}
