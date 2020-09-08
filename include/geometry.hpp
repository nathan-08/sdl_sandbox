#pragma once

#include <vector>
#include <SDL.h>
#include <cstdint>

struct Point{ float x, y; };
namespace Geometry {
  std::vector<Point> getCircle(float r, Point origin);
  void drawCircle( SDL_Renderer *renderer, int32_t centerX, int32_t centerY, int32_t radius);
}
