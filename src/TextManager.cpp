#include "TextManager.hpp"
#include <cctype>
#include <iostream>
#include "constants.hpp"
#include <algorithm>

using namespace std;
static string getCharlist()
{
  string charlist;
  for(int i = 0; i < 0xff; i++) {
    if (isgraph(i)) charlist += i;
  }
  charlist += ' ';
  return charlist;
}

GlyphCache::GlyphCache(SDL_Renderer* r, TTF_Font* f)
  : renderer(r), font(f)
{
  string charlist = getCharlist();
  //string charlist = "0123456789abcdefgh";
  int xoffset = 0;
  //int yoffset = 16*2;
  for(const char& ch : charlist)
  {
    int minx, maxx, miny, maxy, advance;
    TTF_GlyphMetrics(font, ch, &minx, &maxx, &miny, &maxy, &advance);
    glyph_map[ch] = new SDL_Rect {xoffset, 0, advance, CHAR_H};
    xoffset += advance;
    //cout << ch << " width: " << advance << ", height: ";
    //cout << "  " << maxy - miny << endl;
    //glyph_map[ch] = new SDL_Rect {xoffset, yoffset, 8, 16};
    //xoffset += 8;
  }
  //SDL_Surface* s = SDL_LoadBMP("/Users/nathanklundt/fonts/bizcat.bmp");
  //cout << "surface: " << (int)s->format->BitsPerPixel << endl;
  //SDL_SetColorKey(s, SDL_TRUE, SDL_MapRGB(s->format, 0xff, 0xff, 0xff));
  // generate glyphset texture
  SDL_Surface* s1 = TTF_RenderText_Solid(font,
                                        charlist.c_str(),
                                        SDL_Color{255,255,255,255});
  //SDL_Surface* s2 = SDL_ConvertSurface(s, s1->format, 0);
  glyphset = SDL_CreateTextureFromSurface(renderer, s1);
  SDL_SetTextureBlendMode(glyphset, SDL_BLENDMODE_BLEND);
  //SDL_FreeSurface(s);
  SDL_FreeSurface(s1);
  //SDL_FreeSurface(s2);
}

GlyphCache::~GlyphCache()
{
  for (auto& p: glyph_map) {
    if (p.second) delete p.second;
  }
  if(glyphset)
  {
    SDL_DestroyTexture(glyphset);
    glyphset = nullptr;
  }
}

TextArea* TextArea::active_ta_ptr = nullptr;
uint8_t   TextArea::padding = 4;

TextArea::TextArea(SDL_Renderer* r,
                   Rect<int> d_,
                   Rect<uint8_t> textColor_,
                   Rect<uint8_t> bgColor_,
                   const GlyphCache& gc_)
                   : renderer(r)
                   , d(d_)
                   , textColor(textColor_)
                   , bgColor(bgColor_)
                   , gc(gc_)
                   , maxLines(((d.h - TextArea::padding*2 - CHAR_H) / CHAR_H))
                   , maxCharsPerLine((d.w - padding*2) / CHAR_W)
{ }

void TextArea::activate(TextArea& ta) noexcept {
  if (active_ta_ptr)
    active_ta_ptr->active = false;
  ta.active = true;
  active_ta_ptr = &ta;
}

void TextArea::updateLineData() {
  lineData.clear();
  LineDatum line_dat(0,0);
  for (int i = 0; i < text.size(); ++i) {
    const char& ch = text.at(i);
    if (isgraph(ch) || ch == ' ' || ch == '\t') {
      ++line_dat.num_chars;
    }
    if (ch == '\n' || line_dat.num_chars == maxCharsPerLine) {
      lineData.push_back(line_dat);
      line_dat = LineDatum(i+1,0);
    }
  }
  lineData.push_back(line_dat);
}

void TextArea::setText(const string& s) {
  cursor_pos = 0;
  firstLineToRender = 0;
  text = s;
  updateLineData();
}

string TextArea::getText() const { return text; }

void TextArea::append(char ch) {
  if (cursor_pos == text.size())
    text.push_back(ch);
  else {
    text.insert(cursor_pos, 1, ch);
  }
  ++cursor_pos;
  updateLineData();
  updateFLTR();
}

void TextArea::backspace() {
  if (text.empty() || cursor_pos == 0) return;
  text.erase(text.begin() + cursor_pos - 1);
  --cursor_pos;
  updateLineData();
  updateFLTR();
}

void TextArea::curseUp() {
  int cursorLine, cursorOffset;
  getLineAndOffsetFromIdx(cursor_pos, cursorLine, cursorOffset);
  if (cursorLine > 0) {
    if (cursorOffset > furthestOffset) furthestOffset = cursorOffset;
    cursor_pos = lineData.at(cursorLine-1).text_idx;
    cursor_pos += min(furthestOffset,
        (int)lineData.at(cursorLine-1).num_chars);
    updateFLTR();
  }
}
void TextArea::curseDown() {
  int cursorLine, cursorOffset;
  getLineAndOffsetFromIdx(cursor_pos, cursorLine, cursorOffset);
  if (cursorLine < lineData.size() - 1) {
    if (cursorOffset > furthestOffset) furthestOffset = cursorOffset;
    cursor_pos = lineData.at(cursorLine+1).text_idx;
    cursor_pos += min(furthestOffset,
        (int)lineData.at(cursorLine+1).num_chars);
    updateFLTR();
  }
}
void TextArea::curseLeft() {
  int cursorLine, cursorOffset;
  getLineAndOffsetFromIdx(cursor_pos, cursorLine, cursorOffset);
  if (cursorOffset > 0) {
    --cursor_pos;
    furthestOffset = cursorOffset-1;
  }
}
void TextArea::curseRight() {
  int cursorLine, cursorOffset;
  getLineAndOffsetFromIdx(cursor_pos, cursorLine, cursorOffset);
  if (cursorOffset < lineData.at(cursorLine).num_chars){
    ++cursor_pos;
    furthestOffset = cursorOffset+1;
  }
}

void TextArea::updateFLTR() {
  int cursorLine, cursorOffset;
  getLineAndOffsetFromIdx(cursor_pos, cursorLine, cursorOffset);
  if (cursorLine < firstLineToRender) {
    firstLineToRender = cursorLine;
  }
  else if (cursorLine > (firstLineToRender + maxLines)) {
    firstLineToRender = cursorLine - maxLines;
  }
}

void TextArea::getLineAndOffsetFromIdx(int cursor_idx, int& line, int& offset) {
  line = 0;
  offset = 0;

  if (lineData.empty()) updateLineData();

  const LineDatum* line_dat = &lineData.at(0); // this should never be empty
  for (int i = 0; i < lineData.size(); ++i) {
    line_dat = &lineData.at(i);
    if (line_dat->text_idx < cursor_idx) {
      line = i;
    }
    else break;
  }
  offset = cursor_idx - lineData.at(line).text_idx;
  if (offset == maxCharsPerLine ||
      (cursor_idx > 0 && text.at(cursor_idx-1)=='\n'))
  {
    ++line;
    offset = 0;
  }
}

int TextArea::getLLTR() {
  return min((int)lineData.size()-1,
             firstLineToRender + maxLines);
}

void TextArea::zt() {
  int cursorLine, cursorOffset;
  getLineAndOffsetFromIdx(cursor_pos, cursorLine, cursorOffset);
  firstLineToRender = cursorLine;
}

void TextArea::zb() {
  int cursorLine, cursorOffset;
  getLineAndOffsetFromIdx(cursor_pos, cursorLine, cursorOffset);
  firstLineToRender = max(
      0,
      cursorLine - maxLines
      );
}

void TextArea::gg() {
  cursor_pos = 0;
  updateFLTR();
}

void TextArea::G() {
  cursor_pos = text.size();
  updateFLTR();
}

void TextArea::updateSearch(string searchstr) {
  if (text.empty()) return;
  search_indices.clear();
  search_len = searchstr.size();
  size_t idx = -1;
  while ((idx = text.find(searchstr, idx+1)) != string::npos) {
    search_indices.push_back(idx);
  }
}

void TextArea::moveToNextSearchIdx() {
  if (!search_indices.empty()) {
    for (const size_t& i: search_indices) {
      if (i > cursor_pos) {
        cursor_pos = i;
        updateFLTR();
        return;
      }
    }
    cursor_pos = search_indices.at(0);
    updateFLTR();
  }
}

void TextArea::renderPrint()
{
  // render bg
  SDL_SetRenderDrawColor(renderer,
                         bgColor.r,
                         bgColor.g,
                         bgColor.b,
                         bgColor.a);
  SDL_Rect border = d.sdl_rect();
  SDL_RenderFillRect(renderer, &border);
  if (active) {
    // render cursor
    int cursorLine, cursorOffset;
    getLineAndOffsetFromIdx(cursor_pos, cursorLine, cursorOffset);
    int cx = d.x+cursorOffset*CHAR_W + padding;
    int cy = (cursorLine - firstLineToRender)*CHAR_H + padding;

    SDL_Rect cursor{
      cx,
      cy,
      CHAR_W,CHAR_H};
    SDL_SetRenderDrawColor(renderer,
                           textColor.r,
                           textColor.g,
                           textColor.b,
                           textColor.a);
    SDL_RenderFillRect(renderer, &cursor);
  }
  // apply text color
  //cout << "texture color mod" << endl;
  //cout << hex;
  //cout << (int)textColor.r << "." << (int)textColor.g << "." << (int)textColor.b << endl;
  SDL_SetTextureColorMod(gc.glyphset,
                         textColor.r,
                         textColor.g,
                         textColor.b);
  int base_x_offset = d.x + TextArea::padding;
  int base_y_offset = d.y + TextArea::padding;
  int xoffset = base_x_offset;
  int yoffset = base_y_offset;
  int lastLineToRender = getLLTR();

  size_t search_idx_counter = 0;
  size_t search_idx = -1;
  int search_len_counter = -1;
  size_t num_search_idcs = search_indices.size();
  if (active && (num_search_idcs > search_idx_counter)) {
    search_idx = search_indices.at(search_idx_counter++);
  }

  for (int j = firstLineToRender; j <= lastLineToRender; ++j) {
    const auto& line_dat = lineData.at(j);
    for (int i = line_dat.text_idx;
         i < (line_dat.text_idx + line_dat.num_chars);
         ++i)
    {
      const char& ch = text.at(i);
      while (i > search_idx && search_idx_counter < num_search_idcs) {
        search_idx = search_indices.at(search_idx_counter++);
      }
      if (i == search_idx) {
        search_len_counter = search_len;
        if (search_idx_counter < num_search_idcs) {
          search_idx = search_indices.at(search_idx_counter++);
        }
      }
      if (ch == '\t') {
        xoffset += CHAR_W;
      }
      else if (cursor_pos == i && active)
      {
        if (isgraph(ch) || ch == ' ') {
          SDL_SetTextureColorMod(gc.glyphset,
                                 bgColor.r,
                                 bgColor.g,
                                 bgColor.b);
          SDL_Rect* rect = gc.glyph_map.at(ch);
          SDL_Rect destrect = {xoffset, yoffset, rect->w, rect->h };
          SDL_RenderCopy(renderer, gc.glyphset, rect, &destrect);
          xoffset += CHAR_W;
          SDL_SetTextureColorMod(gc.glyphset,
                                 textColor.r,
                                 textColor.g,
                                 textColor.b);
        }
        if (search_len_counter > 0) --search_len_counter;
      }
      else if (isgraph(ch) || ch == ' ') {
        if (active && (search_len_counter > 0)) {
          SDL_SetTextureColorMod(gc.glyphset,
                                 bgColor.r,
                                 bgColor.g,
                                 bgColor.b);
          SDL_SetRenderDrawColor(renderer,
                                 0xc0,
                                 0xc0,
                                 0xf0,
                                 0xff);
          SDL_Rect highlight {xoffset, yoffset, CHAR_W, CHAR_H};
          SDL_RenderFillRect(renderer, &highlight);
          SDL_SetRenderDrawColor(renderer,
                                 bgColor.r,
                                 bgColor.g,
                                 bgColor.b,
                                 bgColor.a);
          SDL_Rect* rect = gc.glyph_map.at(ch);
          SDL_Rect destrect = {xoffset, yoffset, rect->w, rect->h };
          SDL_RenderCopy(renderer, gc.glyphset, rect, &destrect);
          SDL_SetTextureColorMod(gc.glyphset,
                                 textColor.r,
                                 textColor.g,
                                 textColor.b);
          --search_len_counter;
        }
        else {
          SDL_Rect* rect = gc.glyph_map.at(ch);
          SDL_Rect destrect = {xoffset, yoffset, rect->w, rect->h };
          SDL_RenderCopy(renderer, gc.glyphset, rect, &destrect);
        }
        xoffset += CHAR_W;
      }
    }
    if (j != lineData.size()-1) {
      xoffset = base_x_offset;
      yoffset += CHAR_H;
    }
  }
}
