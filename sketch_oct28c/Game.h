#include "Player.h"
#include "Sprites.h"

PlayerClass playerobj;
SpriteClass Objects[MAXOBJECT];

byte ONum = 0;



void Init(int x,int y){
  playerobj.x = x;
  playerobj.y = y;
  playerobj.d = 0;
  playerobj.H = 100;
}

bool Intersect(unsigned Min0, unsigned Max0, unsigned Min1, unsigned Max1){return (Max0 >= Min1) && (Min0 <= Max1);}

bool Collision(unsigned x, unsigned y, unsigned x1, unsigned y1) {return (Intersect(x,x+8,x1,x1+8)&&Intersect(y,y+8,y1,y1+8));}




void DisplayEnviroment()
{
  int tileX = GetTileX(playerobj.x);
  int tileY = GetTileY(playerobj.y);
  int offsetX = GetTileXOffset(playerobj.x);
  int offsetY = GetTileYOffset(playerobj.y);
    
  for (int i = -5; i < 5; i++)
  {
    for(int j = -3; j < 3; j++)
    {
      uint8_t block = GetBlock(tileX + i, tileY + j);
      int drawX = (i * 16) + CENTERX - offsetX;
      int drawY = (j * 16) + CENTERY - offsetY;
      sprites.drawSelfMasked(drawX, drawY, BackgroundEnviroment, block);
    }
  }
}

void DisplayPlayer(){	
  // Debugging Stuff
  /*int tileX = GetTileX(playerobj.x);
  int tileY = GetTileY(playerobj.y);
  int offsetX = GetTileXOffset(playerobj.x);
  int offsetY = GetTileYOffset(playerobj.y);
  
  ard.setCursor(0,0);
  ard.print(F("TX: "));
  ard.println(tileX);
  ard.print(F("TY: "));
  ard.println(tileY);
  ard.print(F("OX: "));
  ard.println(offsetX);
  ard.print(F("OY: "));
  ard.println(offsetY);*/
	
  if ((playerobj.Moving) && (ard.everyXFrames(15))){
        playerobj.Frame = !playerobj.Frame;
        if (playerobj.Frame){
        sprites.drawOverwrite(CENTERX-4,CENTERY-4,SpriteEnviroment,1);
        } else {
        sprites.drawOverwrite(CENTERX-4,CENTERY-4,SpriteEnviroment,2);
        }
  } else {
  sprites.drawOverwrite(CENTERX-4,CENTERY-4,SpriteEnviroment,0);
  }
}

void UpdateMainMenu(){
  sprites.drawSelfMasked(0,0,Logo,0);
  if (showarrow){sprites.drawOverwrite(20,55,Arrow,0);}
  if (ard.justPressed(A_BUTTON)){
    gameState = GameState::LoadMap;
  }
  if (ard.everyXFrames(30)) {
      showarrow = !showarrow;
      }
}

void LoadMAP(byte L){
    const uint8_t * CLevel = Maps[L];
    int px = pgm_read_byte(&CLevel[2]);
    int py = pgm_read_byte(&CLevel[3]);
    uint8_t index = OFFSET+MAP_SIZE+1;
    Init(px,py);
    memcpy_P(&Map[0], &CLevel[OFFSET], MAP_SIZE);
    ONum = pgm_read_byte(&CLevel[index]);
    byte ID = 0;
    byte H = 0;
    byte Offs = 0;
    bool A = false;
    index++;
    for (int i=0; i<MAXOBJECT; i++){
      Objects[i].setSprite(0,0,0,0,0,false);
    }
    for (int i=0; i<MAXENVIROMENT; i++){
      Envi[i].SetEnv(0,0,0,0,false);
    }
    
    for (int i=0; i<ONum; i++){
        ID = pgm_read_byte(&CLevel[index++]);
        px = pgm_read_byte(&CLevel[index++]);
        py = pgm_read_byte(&CLevel[index++]);
        H = pgm_read_byte(&CLevel[index++]);
        switch (ID){
          case 1: Offs=12; break; //Coin
          case 2: Offs=8; break; //Potion
          case 3: Offs=9; break; //Jelly Filled Doughnut
          case 4: Offs=10; break; //Key
          case 5: Offs=11; break; //Ham
          case 6: Offs=4; break; //Monsters VVVV
          case 7: Offs=5; break;
          case 8: Offs=6; break;
        }
        Objects[i].setSprite(px,py,H,ID,Offs,true);
    }
    ENum = pgm_read_byte(&CLevel[index++]);
    for (int i=0; i<ONum; i++){
        ID = pgm_read_byte(&CLevel[index++]);
        Offs = pgm_read_byte(&CLevel[index++]);
        py = pgm_read_byte(&CLevel[index++]);
        H = pgm_read_byte(&CLevel[index++]);
        Envi[i].SetEnv(ID,Offs,py,H,true);
    }
}

void NextLevelLoad(){
  LoadMAP(Level);
  Level++; 
  gameState = GameState::Game;
}

void UpdateObjects(){
  for (byte i=0;i<ONum;i++){
    if (Objects[i].IsActive()) {
      if (Collision(Objects[i].GetX(),Objects[i].GetY(),playerobj.x,playerobj.y)){
          switch(Objects[i].GetType()){
            case 1: playerobj.Coins++; Objects[i].SetActive(false); break;
            case 3: playerobj.H += 5; if (playerobj.H > 100) {playerobj.H = 100;} break;
            case 5: playerobj.H += 10; if (playerobj.H > 100) {playerobj.H = 100;} break;
            case 4: playerobj.Keys++; Objects[i].SetActive(false); break;
            }
      } else {
        Objects[i].UPPos(playerobj.x,playerobj.y);
        Objects[i].SpriteAI();
        }
      }
  }
}

void DisplayObjects() {
  for (byte i=0;i<ONum;i++){
    if (Objects[i].IsActive()) {
    Objects[i].Display();
    }
  }
}


void MapEnding(){
    int padd = playerobj.Coins * 5;    
    int kadd = playerobj.Keys * 10;
    ard.setCursor(0,0);
    ard.print(F("Level:"));
    ard.println(Level);
    ard.print(F("Coins:"));
    ard.println(playerobj.Coins);
    ard.print(F("Keys:"));
    ard.println(playerobj.Keys);
    ard.print(F("Level Points:"));
    ard.println(kadd+padd);
    
    ard.print(F("Total Points:"));
    ard.println(POINTS+kadd+padd);
    
    if (ard.everyXFrames(120)) {
        gameState = GameState::LoadMap;
        POINTS += padd + kadd;
    }
}


void UpdateGame(){
  playerobj.PlayerMovement();
  UpdateObjects();
  DisplayEnviroment();
  DisplayPlayer();
  DisplayObjects();
}

