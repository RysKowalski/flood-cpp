#pragma once

#include "Grid.hpp"
#include <cstdint>

class FloodSim {
public:
  FloodSim(int width, int height);
  void tick();
  std::vector<std::uint8_t> pixels;

private:
  int width;
  int height;
  Grid current;
  Grid next;
  bool floodable(int x, int y) const;
  void update_pixels();
  void update_pixel(int x, int y);
  void process_cell(int x, int y);
};
