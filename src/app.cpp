#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <fstream>
#include <exception>
#include "app.hpp"
#include "TextManager.hpp"
#include "AppErr.hpp"
#include "initialization.hpp"
#include "constants.hpp"
#include <vector>
#include "rect.hpp"

using namespace std;
static SDL_Window *gWindow = NULL;
static SDL_Renderer *gRenderer = NULL;
static TTF_Font *gFont = NULL;
TextArea* tap = nullptr;

string getCharlist(void);

bool App::quit = false;

void App::init() // uses functions declared in initialization.hpp
{
    if (!sdl_init( &gWindow, &gRenderer )) throw AppErr("init failed");
    if (!load_font( &gFont )) throw AppErr("load font failed");
}

string App::filepath;

void App::execute_command(string command) {
  //cout << "executing <" << command << ">" << endl;
  if (command.find(":open ") == 0) {
    // TODO work on this
    ifstream f(command.substr(6), fstream::in | fstream::out);
    if (!f.is_open()) {
      cout << "failed to open " + command.substr(6) << endl;
      return;
    }
    filepath = command.substr(6);
    ostringstream oss;
    oss << f.rdbuf();
    tap->setText(oss.str());
  }
  else if (command == ":q") {
    quit = true;
  }
  else if (command == ":zt") {
    tap->zt();
  }
  else if (command == ":zb") {
    tap->zb();
  }
  else if (command == ":gg") {
    tap->gg();
  }
  else if (command == ":G") {
    tap->G();
  }
}

void App::mainloop()
{
  GlyphCache glyphcache(gRenderer, gFont);
  TextArea ta(gRenderer,
              {0, 0, L_WIDTH, L_HEIGHT - (CHAR_H + TextArea::padding)},
              {0xee, 0xee, 0xee, 0xff},       // text color
              {0x40, 0x60, 0xee, 0xff},       // bg color
              glyphcache);    
  TextArea::activate(ta);
  tap = &ta;
  TextArea sb(gRenderer,
              {0, L_HEIGHT - (CHAR_H + TextArea::padding), L_WIDTH, (CHAR_H + TextArea::padding)},
              {0x40, 0x60, 0xee, 0xff},       // text color
              {0xee, 0xee, 0xee, 0xff},       // bg color
              glyphcache);    
  sb.setText("EDIT MODE");
  enum Mode { EDIT, INSERT };
  enum SubMode { DEFAULT, COMMAND };
  Mode mode = EDIT;
  SubMode subMode = DEFAULT;

  SDL_Event e;

  string inputText("");

  while( !quit )
  {
    if (mode == EDIT) {
      while( SDL_PollEvent( &e ) != 0 )
      {
          if( e.type == SDL_QUIT ) quit = true;
          if (e.type == SDL_TEXTINPUT) {
            if (subMode == DEFAULT) {
              if (e.text.text[0] == ':') {
                subMode = COMMAND;
                sb.setText("");
                sb.append(':');
              }
            }
            else if (subMode == COMMAND) {
              sb.append(e.text.text[0]);
            }
          }
          else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
              case SDLK_i:
                if (subMode == DEFAULT) {
                  mode = INSERT;
                  sb.setText("INSERT MODE");
                  while (SDL_PollEvent(&e) != 0);
                  break;
                }
              case SDLK_k:
                if (subMode == DEFAULT) {
                  ta.curseUp();
                }
                break;
              case SDLK_j:
                if (subMode == DEFAULT) {
                  ta.curseDown();
                }
                break;
              case SDLK_h:
                if (subMode == DEFAULT) {
                  ta.curseLeft();
                }
                break;
              case SDLK_l:
                if (subMode == DEFAULT) {
                  ta.curseRight();
                }
                break;
              case SDLK_RETURN:
                if (subMode == COMMAND) {
                  execute_command(sb.getText());
                  subMode = DEFAULT;
                  sb.setText("EDIT MODE");
                }
                break;
              case SDLK_BACKSPACE:
                if (subMode == COMMAND) {
                  sb.backspace();
                }
            }
          }
      }
    }
    else if (mode == INSERT) {
      while (SDL_PollEvent(&e) != 0) {
        if( e.type == SDL_QUIT )
        { quit = true; }
        if (e.type == SDL_TEXTINPUT) {
          ta.append(e.text.text[0]);
        }
        else if (e.type == SDL_KEYDOWN) {
          switch (e.key.keysym.sym) {
            case SDLK_RETURN:
              ta.append('\n');
              break;
            case SDLK_BACKSPACE:
              ta.backspace();
              break;
            case SDLK_ESCAPE:
              mode = EDIT;
              sb.setText("EDIT MODE");
              while (SDL_PollEvent(&e)!=0); // flush events
          }
        }
      }

    }
    SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 0xff );
    SDL_RenderClear( gRenderer );

    ta.renderPrint();
    sb.renderPrint();

    SDL_RenderPresent( gRenderer );
  }
}

void App::close()
{
  TTF_CloseFont( gFont );
  SDL_DestroyRenderer( gRenderer ); gRenderer = NULL;
  SDL_DestroyWindow( gWindow ); gWindow = NULL;

  TTF_Quit();
  SDL_Quit();
}
