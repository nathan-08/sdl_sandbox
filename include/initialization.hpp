#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

bool sdl_init( SDL_Window **window_p, SDL_Renderer **renderer_p );
bool load_font( TTF_Font **font_p );
