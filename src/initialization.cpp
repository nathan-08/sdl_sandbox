#include "initialization.hpp"
#include "constants.hpp"
#include <SDL_mixer.h>

bool sdl_init( SDL_Window **window_p, SDL_Renderer **renderer_p )
{
  bool success = true;

  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
  {
    printf( "SDL could not initialize! Error: %s\n", SDL_GetError() );
    success = false;
  }

  *window_p = SDL_CreateWindow( "SDL FONT DEMO", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
  if( *window_p == NULL )
  {
    printf( "Window could not be created! Error: %s\n", SDL_GetError() );
    success = false;
  }

  *renderer_p = SDL_CreateRenderer( *window_p, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
  if( *renderer_p == NULL )
  {
    printf( "Renderer could not be created! Error: %s\n", SDL_GetError() );
    success = false;
  }

  if( TTF_Init() == -1 )
  {
    printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
    success = false;
  }

  if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
    printf( "SDL_mixer failed to init: %s\n", Mix_GetError() );
    success = false;
  }
  SDL_RenderSetLogicalSize( *renderer_p, L_WIDTH, L_HEIGHT );

  return success;
}

bool load_font( TTF_Font **font_p )
{
  bool success = true;
  //// gb.ttf, press_start.ttf, 04b_03.ttf ////
  //gFont = TTF_OpenFont( "fonts/pc_fonts/ttf-mx-mixed-outline+bitmap/Mx437_ATI_8x8.ttf", 8);
  *font_p = TTF_OpenFont( "fonts/04b_03.ttf", 8);
  if( *font_p == NULL )
  {
    printf( "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError() );
    success = false;
  }
  return success;
}
