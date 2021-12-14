#ifndef _RECT_H
#define _RECT_H

template<typename T>
struct Rect {
  T  x, y, w, h;
  T& r, g, b, a;
public:
  Rect(T a, T b, T c, T d)
    : x(a), y(b), w(c), h(d),
      r(x), g(y), b(w), a(h)
  {}
  inline SDL_Rect sdl_rect() const { return { x, y, w, h }; }
};

#endif
