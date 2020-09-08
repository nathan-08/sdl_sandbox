#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <ctype.h>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <exception>
#include "app.hpp"
#include "TextManager.hpp"
#include "Clock.hpp"
#include "http.hpp"
#include "AppErr.hpp"
#include "initialization.hpp"
#include "constants.hpp"
#include <vector>
#include "geometry.hpp"
#include <bitset>

static SDL_Window *gWindow = NULL;
static SDL_Renderer *gRenderer = NULL;
static TTF_Font *gFont = NULL;

enum Direction { UP = 1 << 0, DOWN = 1 << 1, LEFT = 1 << 2, RIGHT = 1 << 3 };
struct Movement {
  uint8_t dir = DOWN;
  const int VEL = 1;
  int velX = 0;
  int velY = 0;
  void move() {
    static uint8_t counter = 0;
    counter++;
    drect.x += velX;
    drect.y += velY;
    if( counter % 4 == 0 ) {
      if( dir & ( DOWN | UP ) ) {
        if( velX | velY ) {
          srect.x = srect.x <= 0x08 ? 0x10 : 0x00;
        } else {
          srect.x = 0x08;
        }
      } else if( dir & ( LEFT | RIGHT ) ) {
        if( velX | velY ) {
          srect.x = srect.x == 0x00 ? 0x08 : 0x00;
        } else {
          srect.x = 0x00;
        }
      }


      //if( velX | velY ) {
        //srect.x = srect.x == 0x18 ? 0x00 : srect.x + 0x08;
      //} else {
        //if( dir == UP || dir == DOWN )
          //srect.x = 0x08;
        //else
          //srect.x = 0x00;
      //}
    }
    switch( dir ) {
      case UP:
        srect.y = 0x08;
        break;
      case DOWN:
        srect.y = 0x00;
        break;
      case LEFT:
        srect.y = 0x18;
        break;
      case RIGHT:
        srect.y = 0x10;
        break;
    }
  }
  SDL_Rect *getDestRect() { return &drect; }
  SDL_Rect *getSourceRect() { return &srect; }
  Movement( int x, int y ): drect{ x, y, 8, 8 } {}
  Movement() = default;

  SDL_Rect srect{ 0, 0, 8, 8 };
  SDL_Rect drect{ 0, 0, 8, 8 };
};

void App::init() // uses functions declared in initialization.hpp
{
    if (!sdl_init( &gWindow, &gRenderer )) throw AppErr("init failed");
    if (!load_font( &gFont )) throw AppErr("load font failed");
}

void App::mainloop()
{
    // load spritesheet
    SDL_Surface* dude_sheet = SDL_LoadBMP("../assets/dood_sprites.bmp");
    if(!dude_sheet)
      throw std::runtime_error("failed to load dood_sprites.bmp");
    SDL_SetColorKey( dude_sheet, SDL_TRUE, SDL_MapRGB( dude_sheet->format, 0, 0, 0 ) );

    SDL_Texture* sheet_texture = SDL_CreateTextureFromSurface( gRenderer, dude_sheet );
    if(!sheet_texture)
      throw std::runtime_error("failed to create sheet texture");

    SDL_FreeSurface( dude_sheet );

    std::ostringstream oss;

    TextManager::TextArea mainTextArea( gRenderer, 0, 0, L_WIDTH - 8, 8 );
    TextManager::TextArea statusTextArea( gRenderer, 0, 8, L_WIDTH - 8, 8 );
    TextManager::TextArea bits( gRenderer, 0, 16, L_WIDTH - 8, 8 );

    TextManager::GlyphCache glyphcache( gRenderer, gFont );
    std::string timestr = Clock::getTime();
    std::string weather = HTTP::get_weather();
    bool quit = false;
    SDL_Event e;
    //SDL_StartTextInput();
    std::string inputText = "";
    unsigned frame_counter = 0;
    Movement m( L_WIDTH - 8, L_HEIGHT - 8 );

    // setup music
    Mix_Chunk *coinSound = Mix_LoadWAV( "../assets/audio/coin.wav" );
    Mix_Chunk *powerupSound = Mix_LoadWAV( "../assets/audio/powerup.wav" );
    if( !coinSound || !powerupSound ) throw new std::runtime_error( "Failed to load audio\n" );

    while( !quit )
    {
        while( SDL_PollEvent( &e ) != 0 )
        {
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
            if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
            {
              switch( e.key.keysym.sym )
              {
                case SDLK_s:
                              Mix_PlayChannel( -1, coinSound, 0 );
                              break;
                case SDLK_a:
                              Mix_PlayChannel( -1, powerupSound, 0 );
                              break;
                case SDLK_k:
                case SDLK_UP: m.velY -= m.VEL;
                              m.dir |= UP;
                              break;
                case SDLK_j:
                case SDLK_DOWN: m.velY += m.VEL;
                                m.dir |= DOWN;
                                break;
                case SDLK_h:
                case SDLK_LEFT: m.velX -= m.VEL;
                                m.dir |= LEFT;
                                break;
                case SDLK_l:
                case SDLK_RIGHT: m.velX += m.VEL;
                                 m.dir |= RIGHT;
                                 break;
              }
            }
            else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
            {
              switch( e.key.keysym.sym )
              {
                case SDLK_k:
                case SDLK_UP: m.velY += m.VEL;
                              m.dir &= ~UP;
                              break;
                case SDLK_j:
                case SDLK_DOWN: m.velY -= m.VEL;
                                m.dir &= ~DOWN;
                                break;
                case SDLK_h:
                case SDLK_LEFT: m.velX += m.VEL;
                                m.dir &= ~LEFT;
                                break;
                case SDLK_l:
                case SDLK_RIGHT: m.velX -= m.VEL;
                                m.dir &= ~RIGHT;
                                 break;
              }
            }
            //else if( e.type == SDL_KEYDOWN )
            //{
                //if( e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0 ) { inputText.pop_back(); }
                //else if( e.key.keysym.sym == SDLK_RETURN ) { inputText += '\n'; }
            //}
            //else if( e.type == SDL_TEXTINPUT )
            //{
                //inputText += e.text.text;
            //}
        }

        // update time
        if (frame_counter % 60 == 0)
        {
          timestr = Clock::getTime();
        }
        if (frame_counter % (60*60*5) == 0)
        {
          weather = HTTP::get_weather();
        }

        SDL_SetRenderDrawColor( gRenderer, 0x1a, 0x34, 0x61, 0xFF );
        SDL_RenderClear( gRenderer );

        SDL_SetRenderDrawColor( gRenderer, 0xff, 0xff, 0xff, 0xFF );

        oss.str(""); oss.clear();
        oss << static_cast<std::bitset<4>>(m.dir) << std::endl;

        mainTextArea.renderPrint( glyphcache, weather.c_str() );
        statusTextArea.renderPrint( glyphcache, timestr.c_str() );
        bits.renderPrint( glyphcache, oss.str().c_str() );

        if (frame_counter % 2 == 0) m.move();
        SDL_RenderCopy( gRenderer, sheet_texture, m.getSourceRect(), m.getDestRect() );

        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0, 0, 0xFF );
        //Geometry::drawCircle( gRenderer, 40, 40, 10 );

        SDL_RenderPresent( gRenderer );
        ++frame_counter;
    }

    SDL_DestroyTexture( sheet_texture );
    Mix_FreeChunk( coinSound ); coinSound = nullptr;
    Mix_FreeChunk( powerupSound ); powerupSound = nullptr;
}

void App::close()
{
  TTF_CloseFont( gFont );
  SDL_DestroyRenderer( gRenderer ); gRenderer = NULL;
  SDL_DestroyWindow( gWindow ); gWindow = NULL;

  TTF_Quit();
  SDL_Quit();
  Mix_Quit();
}
