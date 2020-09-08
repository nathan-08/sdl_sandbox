#ifndef TEXT_MANAGER_HPP
#define TEXT_MANAGER_HPP

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include <map>

namespace TextManager
{
  class GlyphCache {
    private:
      SDL_Renderer *renderer;
      TTF_Font *font;

    public:
      std::map<char, SDL_Rect*> glyph_map;

      SDL_Texture *glyphset = NULL; // glyph atlas

      GlyphCache(SDL_Renderer *r, TTF_Font *f );

      ~GlyphCache();

      SDL_Rect *getGlyphRect(const char ch);
  };

  class TextArea {
    private:
      SDL_Renderer *renderer;
      int _x, _y, _w, _h;

    public:
      TextArea( SDL_Renderer *r, int x, int y, int w, int h );

      void renderPrint( const GlyphCache &gc, const char *text );

      void drawRect();
  };
}

#endif // TEXT_MANAGER_HPP
