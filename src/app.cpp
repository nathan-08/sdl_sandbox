#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <cctype>
#include <cstdint>
#include <cstdlib>
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
TextArea* sbrp = nullptr;

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
    ifstream f(command.substr(6), fstream::in);
    //if (!f.is_open()) {
      //cout << "failed to open " + command.substr(6) << endl;
      //return;
    //}
    filepath = command.substr(6);
    sbrp->setText(filepath);
    if (f.is_open()) {
      ostringstream oss;
      oss << f.rdbuf();
      tap->setText(oss.str());
    }
    else tap->setText("");
  }
  else if (command == ":w") {
    if (!filepath.empty()) {
      ofstream f(filepath, fstream::trunc);
      if (!f.is_open()) {
        cout << "Failed to write to " + filepath << endl;
        return;
      }
      f << tap->getText();
    }
  }
  else if (command == ":q") {
    quit = true;
  }
  else if (command.find(":!") == 0) {
    // execute system command
    string cmd = command.substr(2);
    system((cmd + " > temp.txt").c_str());
    ostringstream oss;
    oss << ifstream("temp.txt").rdbuf();
    filepath = "temp.txt";
    sbrp->setText(filepath);
    tap->setText(oss.str());
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

bool App::evaluate_quick_buf(string buf, SDL_Keymod keymod) {
  bool is_cap = (keymod & (KMOD_LSHIFT | KMOD_RSHIFT));
  bool ctrl = (keymod & (KMOD_LCTRL | KMOD_RCTRL));
  if (buf.size() > 2) {
    return false;
  }
  else if (buf == "zt") {
    tap->zt();
    return false;
  }
  else if (buf == "zb") {
    tap->zb();
    return false;
  }
  else if (buf == "zz") {
    tap->zz();
    return false;
  }
  else if (buf == "gg") {
    tap->gg();
    return false;
  }
  else if (buf == "o" && is_cap) {
    tap->O();
    return false;
  }
  else if (buf == "o") {
    tap->o();
    return false;
  }
  else if (buf == "g" && is_cap) {
    tap->G();
    return false;
  }
  else if (buf == "dd") {
    tap->dd();
    return false;
  }
  else if (buf == "u" && ctrl) {
    tap->ctrl_u();
    return false;
  }
  else if (buf == "d" && ctrl) {
    tap->ctrl_d();
    return false;
  }
  else return true;
}

void App::mainloop()
{
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
  GlyphCache glyphcache(gRenderer, gFont);
  TextArea ta(gRenderer,
              {0, 0, L_WIDTH, L_HEIGHT - (CHAR_H + TextArea::padding)},
              {0xee, 0xee, 0xee, 0xff},       // text color
              {0x40, 0x40, 0x80, 0xff},       // bg color
              //{0x40, 0x60, 0xee, 0xff},       // bg color (cfblue)
              glyphcache);    
  TextArea::activate(ta);
  tap = &ta;
  TextArea sb(gRenderer,
              {0, L_HEIGHT - (CHAR_H + TextArea::padding), L_WIDTH-200, (CHAR_H + TextArea::padding)},
              //{0x40, 0x60, 0xee, 0xff},       // text color
              {0x40, 0x40, 0x80, 0xff},       // text color
              {0xee, 0xee, 0xee, 0xff},       // bg color
              glyphcache);    
  sb.setText("EDIT MODE");
  TextArea sbr(gRenderer,
      {L_WIDTH - 200, L_HEIGHT - (CHAR_H + TextArea::padding),
      200,
      (CHAR_H + TextArea::padding)},
      {0xff,0xff,0xff,0xff},// text color
      {0x00,0x00,0x00,0xff},// bg color
      glyphcache);
  sbrp = &sbr;
  //sbr.setText("some info here");
  enum Mode { EDIT, INSERT };
  enum SubMode { DEFAULT, COMMAND, SEARCH, QUICK }; // command submode is triggered by entering ':', search by '/'
  // QUICK command is triggered by any other character and can process multiple char sequences.
  string sequence_first_chars = "gGzdHMLou";
  Mode mode = EDIT;
  SubMode subMode = DEFAULT;
  SDL_Keymod keymod;
  string quick_buf;

  SDL_Event e;

  string inputText("");

  while( !quit )
  {
    if (mode == EDIT) {
      while( SDL_PollEvent( &e ) != 0 )
      {
          if( e.type == SDL_QUIT ) quit = true;
          if (e.type == SDL_TEXTINPUT) { // can get mods?
            if (subMode == DEFAULT) {
              if (e.text.text[0] == ':') {
                subMode = COMMAND;
                sb.setText("");
                sb.append(':');
              }
              else if (e.text.text[0] == '/') {
                subMode = SEARCH;
                sb.setText("");
                sb.append('/');
              }
            }
            else if (subMode == COMMAND || subMode == SEARCH) {
              sb.append(e.text.text[0]);
              if (subMode == SEARCH)
                ta.updateSearch(sb.getText().substr(1));
              //if (subMode == QUICK)
                //if (!evaluate_quick_buf(sb.getText(), SDL_GetModState())) {
                  //sb.setText("EDIT MODE");
                  //subMode = DEFAULT;
                //}
            }
          }
          else if (e.type == SDL_KEYDOWN) {
            if (subMode == QUICK || (subMode == DEFAULT && sequence_first_chars.find(e.key.keysym.sym) != string::npos))
            {
              if (subMode == DEFAULT) {
                subMode = QUICK;
                sb.setText("");
              }
              sb.append(e.key.keysym.sym);
              if (!evaluate_quick_buf(sb.getText(), SDL_GetModState())) {
                sb.setText("EDIT MODE");
                subMode = DEFAULT;
              }
            }
            else switch (e.key.keysym.sym) {
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
              case SDLK_n:
                if (subMode == DEFAULT) {
                  ta.moveToNextSearchIdx();
                }
                break;
              case SDLK_RETURN:
                if (subMode == COMMAND || subMode == SEARCH) {
                  if (subMode == COMMAND)
                    execute_command(sb.getText());
                  subMode = DEFAULT;
                  sb.setText("EDIT MODE");
                }
                break;
              case SDLK_BACKSPACE:
                if (subMode == COMMAND || subMode == SEARCH) {
                  sb.backspace();
                }
                break;
              case SDLK_ESCAPE:
                subMode = DEFAULT;
                sb.setText("EDIT MODE");
                break;
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
            case SDLK_TAB:
              ta.append('\t');
              break;
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
    sbr.renderPrint();

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
