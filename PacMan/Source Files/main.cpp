#include "Game.hpp"

int main(){
    Game game;
    
    //Game Loop
    while(game.running()){
        
        game.update();
        game.render();
        
    }
    
    return 0;
}
