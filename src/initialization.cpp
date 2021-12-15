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

  *window_p = SDL_CreateWindow( "vim_x", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
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

  SDL_RenderSetLogicalSize( *renderer_p, L_WIDTH, L_HEIGHT );

  return success;
}

bool load_font( TTF_Font **font_p )
{
  bool success = true;
  //// gb.ttf, press_start.ttf, 04b_03.ttf ////
  //*font_p = TTF_OpenFont("/Users/nathanklundt/fonts/kongtext/kongtext.ttf", 16);

  //*font_p = TTF_OpenFont("/Users/nathanklundt/fonts/pc_fonts/ttf-mx-mixed-outline+bitmap/Mx437_IBM_VGA_8x16.ttf", 16);

  //*font_p = TTF_OpenFont("/Users/nathanklundt/fonts/pc_fonts/ttf-mx-mixed-outline+bitmap/Mx437_PhoenixEGA_8x16.ttf", 16);

  *font_p = TTF_OpenFont("/Users/nathanklundt/fonts/AtariST8x16SystemFont.ttf", 16);


  //*font_p = TTF_OpenFont("/Users/nathanklundt/fonts/pc_fonts/ttf-mx-mixed-outline+bitmap/Mx437_ToshibaTxL1_8x16.ttf", 16);

  if( *font_p == NULL )
  {
    printf( "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError() );
    success = false;
  }
  return success;
}
