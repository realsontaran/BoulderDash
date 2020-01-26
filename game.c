#include  <stdio.h>
#include  <stdlib.h>
#include  <stdbool.h>

#include  "./Lib/include/SDL.h"
#include  "./Lib/include/SDL_image.h"
#include  "./Lib/include/SDL_ttf.h"
#include  "./Lib/include/SDL_mixer.h"

#include "LevelEditor.h"
#include "prototypes.h"

bool isRunning;

SDL_Window *window;
SDL_Renderer *renderer;

Mix_Music *gameMusic;

Mix_Chunk *walk_empty;
Mix_Chunk *boulder_fall;
Mix_Chunk *walk_dirt;
Mix_Chunk *collect_diamond;

TTF_Font *gameFont;
SDL_Color textColor = {0,0,0};

SDL_Surface *text;
SDL_Texture *GameBar;

SDL_Surface *diam;
SDL_Texture *di;

SDL_Surface *life;
SDL_Texture *heart;

SDL_Texture *score_board;
BLOCK blocks[9];

int SCREEN_WIDTH = 900;
int SCREEN_HEIGHT = 600;

int map[100][100];

const int FPS = 60;

int StartTime;
int lastTime = 0;

int game_time;

int previousTime = 0;

int main(int argc, char const *argv[]) 
{

  Uint32 frameStart;
  
  int frameTime;
  int frameDelay = 1000/FPS;


  init_game("Boulder Dash",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,900,600);

  Mix_PlayingMusic();
  Mix_PlayMusic(gameMusic,-1);

  while (isRunning)
  {
    frameStart = SDL_GetTicks();
    StartTime = SDL_GetTicks();

    if (StartTime > lastTime + 1000)
    {
      game_time = StartTime/1000;
      lastTime = StartTime;
      game_time -= previousTime;
    }
    handle_events();
    update();
    render();
    
    frameTime = SDL_GetTicks() -frameStart;
    if (frameDelay > frameTime)
    {
      SDL_Delay(frameDelay-frameTime);
    }

  }
  clean();
  return 0;
}

void init_game(char *title, int xpos, int ypos, int width, int height )
{

  if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
  {
    window = SDL_CreateWindow(title,xpos,ypos,width,height,SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window,-1,0);

    Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 );
   
    gameMusic = Mix_LoadMUS("Sounds/gameMusic.wav");
    walk_empty = Mix_LoadWAV("Sounds/walk_empty.wav");
    walk_dirt = Mix_LoadWAV("Sounds/walk_dirt.wav");
    boulder_fall = Mix_LoadWAV("Sounds/boulder.wav");
    collect_diamond = Mix_LoadWAV("Sounds/diamond.wav");
    
    TTF_Init();

    gameFont = TTF_OpenFont("GameFont/boulderdash.ttf",30);

    isRunning = true;
  }

  set_miner();
  CreateMap(map,1);
}



void set_miner()
{ 
  map[1][1] = miner;
  blocks[miner].xpos = CellSize;
  blocks[miner].ypos = CellSize;

}

void showGameBar(char *t,char *diamond_count, char *life_count, char *game_score)
{
  text = TTF_RenderText_Solid(gameFont,t,textColor);
  GameBar = SDL_CreateTextureFromSurface(renderer,text);

  diam = TTF_RenderText_Solid(gameFont,diamond_count,textColor);
  di = SDL_CreateTextureFromSurface(renderer,diam);

  life = TTF_RenderText_Solid(gameFont,life_count,textColor);
  heart = SDL_CreateTextureFromSurface(renderer,life);

  SDL_Surface *Score = TTF_RenderText_Solid(gameFont,game_score,textColor);

  score_board = SDL_CreateTextureFromSurface(renderer,Score);
}

void handle_events()
{
  SDL_Event event;
  SDL_PollEvent(&event);

  switch (event.type)
  {
    case SDL_QUIT:
      isRunning = false;
      break;
    case SDL_KEYDOWN:
      switch (event.key.keysym.scancode)
      {
        case SDL_SCANCODE_ESCAPE:
          isRunning = false;
          break;

        case SDL_SCANCODE_UP:
          moveU();
          break;

        case SDL_SCANCODE_DOWN:
          moveD();
          break;

        case SDL_SCANCODE_LEFT:
          if (map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize - 1] == rock && map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize - 2] == empty)
          {
            move_rockL();
          }
          if (map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize - 1] != rock && map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize - 1] != border)
          {
            moveL();
          }
          break;

        case SDL_SCANCODE_RIGHT:
          
          if (map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize + 1] == rock && map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize + 2] == empty )
          {
            move_rockR();
          }
          
          if (map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize + 1] != rock && map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize + 1] != border)
          {
            moveR();
          }
          break;

        default:
          break;
      }
    default:
      break;
  }
}

void update()
{
  char time_count[5];
  char diamond_count[5];
  char life_count[2];
  char score_count[10];

  blocks[miner].dest.x = blocks[miner].xpos;
  blocks[miner].dest.y = blocks[miner].ypos;

  for (int rowy = 0; rowy < 100; rowy++)
  {
    for (int colx = 0; colx < 100; colx++)
    {
     
      if ((map[rowy][colx] == dirt) && blocks[miner].xpos/CellSize == colx && blocks[miner].ypos/CellSize == rowy)
      {
        map[rowy][colx] = empty;
      }
      if (map[rowy][colx] == rock && map[rowy + 1][colx] == empty)
      {        
        map[rowy][colx] = empty;
        map[rowy + 1][colx] = rock;

        if (map[rowy + 2][colx] != empty)
        {
          Mix_PlayChannel(-1,boulder_fall,0);
        }
        

        if (map[rowy + 2][colx] == miner)
        {
          set_miner();
          map[rowy + 1][colx] = empty;
          map[rowy + 2][colx] = rock;
          --current_level.level_life;
        }        
        
      }      
      if (map[rowy][colx] == diamond && map[rowy + 1][colx] == empty )
      {
        map[rowy][colx] = empty;
        map[rowy + 1][colx] = diamond;
      }
    }
  }

  temp.x = (blocks[miner].xpos + CellSize) - SCREEN_WIDTH / 2;
  temp.y = (blocks[miner].ypos + CellSize) - SCREEN_HEIGHT / 2;

 if( temp.x < 0 )
  { 
    temp.x = 0;
  }
  if( temp.y < 0)
  {
    temp.y = 0;
  }
  if( temp.x > current_level.levelWidth - temp.w )
  {
    temp.x = current_level.levelWidth - temp.w;
  }
  if( temp.y > current_level.levelHeight - temp.h )
  {
    temp.y = current_level.levelHeight - temp.h;
  }

  if(current_level.level_life == -1 )
  {
      current_level.level_life = 5;
      set_miner();
      CreateMap(map,(map[99][99])/10);    
  }
  

  sprintf(time_count,"%d",current_level.TotalTime - game_time);
  sprintf(diamond_count,"%d",current_level.TotalDiamond);
  sprintf(life_count,"%d",current_level.level_life);
  sprintf(score_count,"%.5d",current_level.TotalDiamond * 10 + (current_level.TotalTime - game_time) * 10);
  showGameBar(time_count,diamond_count,life_count,score_count);
}

void nextLevel()
{
  
  if (map[99][99] == 10 && map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize] == exit_door)
  {
    set_miner();
    CreateMap(map,2);
    previousTime = game_time;
  }
  if (map[99][99] == 20 && map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize] == exit_door)
  {
    set_miner();
    CreateMap(map,3);
  }
}

void CollectDiamonds()
{
  if (map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize] == diamond)
  {
    Mix_PlayChannel(-1,collect_diamond,0);
    
    current_level.TotalDiamond -= 1;
    
    if (current_level.TotalDiamond == 0)
    {
      if (map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize + 1] == border)
      {
        map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize - 1] = exit_door;
      }
      else
      {
        map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize + 1] = exit_door;
      }    
    }
  }   
}
void walkSounds()
{
  if (map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize] == dirt)
  {
    Mix_PlayChannel(-1,walk_dirt,0);
  }
  if (map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize] == empty)
  {
    Mix_PlayChannel(-1,walk_empty,0);
  }
}

void moveR()
{
  blocks[miner].xpos += CellSize;
  
  CollectDiamonds();
  walkSounds();
  nextLevel();
  
  map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize] = miner;
  map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize - 1] = empty;

}

void move_rockR()
{
  blocks[miner].xpos += CellSize;
  map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize] = miner;
  map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize + 1] = rock;
  map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize - 1] = empty;
}

void moveL()
{
  blocks[miner].xpos -= CellSize;

  if (blocks[miner].xpos <= 0 )
  {
    blocks[miner].xpos = CellSize;
  }
  

  CollectDiamonds();
  walkSounds();
  nextLevel();

  map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize] = miner;
  map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize + 1] = empty;  
}

void move_rockL()
{
  blocks[miner].xpos -= CellSize;
  map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize] = miner;
  map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize -1] = rock;
  map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize + 1] = empty;
}

void moveU()
{
  if (map[blocks[miner].ypos/CellSize - 1][blocks[miner].xpos/CellSize] != rock && map[blocks[miner].ypos/CellSize - 1][blocks[miner].xpos/CellSize] != border)
  {
    blocks[miner].ypos -= CellSize;
    
    if (blocks[miner].ypos <= 0 )
    {
      blocks[miner].ypos = CellSize;
    }

    CollectDiamonds();
    walkSounds();
    nextLevel();

    map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize] = miner;
    map[blocks[miner].ypos/CellSize + 1][blocks[miner].xpos/CellSize] = empty;
  }
}

void moveD()
{
  if (map[blocks[miner].ypos/CellSize + 1][blocks[miner].xpos/CellSize] != rock && map[blocks[miner].ypos/CellSize + 1][blocks[miner].xpos/CellSize] != border)
  {
    blocks[miner].ypos += CellSize;
    
    CollectDiamonds();
    walkSounds();
    nextLevel();

    map[blocks[miner].ypos/CellSize][blocks[miner].xpos/CellSize] = miner;
    map[blocks[miner].ypos/CellSize - 1][blocks[miner].xpos/CellSize] = empty;
  }
}

void render()
{
  SDL_Rect clock = {270,0,30,30};
  SDL_Rect t = {300,0,60,30};

  SDL_Rect d = {420,0,30,30};
  SDL_Rect diamond_pos = {450,0,60,30};

  SDL_Rect l = {570,0,30,30};
  SDL_Rect life_count = {600,0,30,30};

  SDL_Rect score = {0,0,900,600};
  SDL_Rect s = {480,310,60,50};
  
  SDL_RenderClear(renderer);
  DrawMap(map,blocks,renderer);
  SDL_RenderSetViewport(renderer,&Camera);
  SDL_RenderCopy(renderer,timer,NULL,&clock);
  SDL_RenderCopy(renderer,GameBar,NULL,&t);
  SDL_RenderCopy(renderer,blocks[diamond].tex,NULL,&d);
  SDL_RenderCopy(renderer,di,NULL,&diamond_pos);
  SDL_RenderCopy(renderer,miner_life,NULL,&l);
  SDL_RenderCopy(renderer,heart,NULL,&life_count);

  if (current_level.TotalTime - game_time <= 0)
  {
    SDL_RenderCopy(renderer,score_screen,NULL,&score);
    SDL_RenderCopy(renderer,score_board,NULL,&s);
    
    if (current_level.TotalTime - game_time == -5)
    {
      isRunning = false;
    }
    
  }
  
  SDL_RenderPresent(renderer);
}

void clean()
{
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);

  Mix_FreeChunk(walk_dirt);
  Mix_FreeChunk(walk_empty);
  Mix_FreeChunk(boulder_fall);
  Mix_FreeMusic(gameMusic);
  Mix_CloseAudio();

  SDL_Quit();
}
