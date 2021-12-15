#ifndef TEXT_MANAGER_HPP
#define TEXT_MANAGER_HPP

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include <map>
#include <vector>
#include <cstdint>
#include <rect.hpp>

class GlyphCache {
    SDL_Renderer* renderer;
    TTF_Font* font;

  public:
    std::map<char, SDL_Rect*> glyph_map;
    SDL_Texture* glyphset = nullptr; // glyph atlas
    GlyphCache(SDL_Renderer* r, TTF_Font* f );
    ~GlyphCache();
};

class TextArea {
    static TextArea* active_ta_ptr;
    bool active = false;
    SDL_Renderer *renderer;
    Rect<int>     d; // x,y,w,h
    Rect<uint8_t> textColor;
    Rect<uint8_t> bgColor;
    std::string text;
    const GlyphCache& gc;

    mutable int firstLineToRender = 0;
    size_t cursor_pos = 0;
    const int maxLines;
    const int maxCharsPerLine;
    int furthestOffset = 0;

    std::vector<size_t> search_indices;
    size_t search_len=0;

    struct LineDatum {
      int text_idx = 0;
      int num_chars = 0;
      int cursor_pos = 0;
      LineDatum(int idx) : text_idx(idx) { }
      LineDatum(int idx, int n) : text_idx(idx), num_chars(n) { }
    };

    std::vector<LineDatum> lineData;

    std::string getTextToRender(); // uses line data to get substring
    void getLineAndOffsetFromIdx(int idx, int& line, int& offset);
    int getLLTR();
  public:
    static uint8_t padding;
    static void activate(TextArea&) noexcept;
    TextArea(SDL_Renderer*,
             Rect<int>,
             Rect<uint8_t>,
             Rect<uint8_t>,
             const GlyphCache&);
    void setText(const std::string& s);
    std::string getText() const;
    void renderPrint();
    void updateLineData();
    void append(char);
    void backspace();
    void curseUp();
    void curseDown();
    void curseLeft();
    void curseRight();
    void updateFLTR();
    void zt();
    void zb();
    void gg();
    void G();
    void updateSearch(std::string);
    void moveToNextSearchIdx();
};

#endif // TEXT_MANAGER_HPP
