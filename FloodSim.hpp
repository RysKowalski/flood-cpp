#pragma once

#include "Grid.hpp"
#include <cstdint>

class FloodSim {
public:
  FloodSim(int width, int height);
  void tick();

private:
  int width;
  int height;
  Grid current;
  Grid next;
  std::vector<std::uint8_t> pixels;
  bool floodable(int x, int y) const;
};
