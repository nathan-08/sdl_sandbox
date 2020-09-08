#include "TextManager.hpp"
#include <stdio.h>
#include <iostream>

static std::string getCharlist()
{
  std::string charlist;
  for(int i = 0; i < 255; i++) {
    if (isgraph(i)) charlist += i;
  }
  charlist += ' ';
  return charlist;
}

TextManager::GlyphCache::GlyphCache(SDL_Renderer *r, TTF_Font *f):
  renderer(r),
  font(f)
{
  std::string charlist = getCharlist();
  int xoffset = 0;
  for(const char& ch : charlist)
  {
    int minx, maxx, miny, maxy, advance;
    TTF_GlyphMetrics( font, ch, &minx, &maxx, &miny, &maxy, &advance );
    glyph_map[ch] = new SDL_Rect { xoffset, 0, advance, 8 };
    xoffset += advance;
  }
  // generate glyphset texture
  SDL_Surface *s = TTF_RenderText_Solid( font, charlist.c_str(), SDL_Color{255,255,255,255} );
  glyphset = SDL_CreateTextureFromSurface( renderer, s );
  SDL_FreeSurface( s );
}

TextManager::GlyphCache::~GlyphCache()
{
  if(glyphset != NULL)
  {
    SDL_DestroyTexture(glyphset);
    glyphset = NULL;
  }
}

TextManager::TextArea::TextArea( SDL_Renderer *r, int x, int y, int w, int h ):
  renderer(r), _x(x), _y(y), _w(w), _h(h) { }

void TextManager::TextArea::drawRect()
{
  SDL_Rect border { _x, _y, _w, _h };
  SDL_RenderDrawRect( renderer, &border );
}

void TextManager::TextArea::renderPrint( const TextManager::GlyphCache &gc, const char *text )
{
  int base_x_offset = _x + 0;
  int base_y_offset = _y + 0;
  int xoffset = base_x_offset;
  int yoffset = base_y_offset;
  for (int i = 0; text[i] != '\0'; i++) {

    if (text[i] == '\n')
    {
      yoffset += 8;
      xoffset = base_x_offset;
      continue;
    }

    SDL_Rect *rect = gc.glyph_map.at(text[i]);
    SDL_Rect destrect = { xoffset, yoffset, rect->w, rect->h };
    SDL_RenderCopy( renderer, gc.glyphset, rect, &destrect );
    xoffset += rect->w;
  } 
  //SDL_Rect cursor = { xoffset, yoffset, 4, 8, };
  //SDL_RenderFillRect( renderer, &cursor );
}
