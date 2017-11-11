#include <Arduboy2.h>
Arduboy2 ard;
Sprites sprites;
int POINTS = 0;
#include "Global.h"
#include "Game.h"





void setup() {
  ard.begin();
  ard.clear();
  Level = 0;
  
}



void loop() {
  if(!ard.nextFrame())
    return;

  ard.pollButtons();
  ard.clear();

  switch(gameState)
  {
    case GameState::MainMenu: UpdateMainMenu(); break;
    //case GameState::Settings: UpdateSettings(); break;
    case GameState::Game: UpdateGame(); break;
    case GameState::LoadMap: NextLevelLoad(); break;
    case GameState::MapEnding: MapEnding(); break;
  }

  ard.display();
}