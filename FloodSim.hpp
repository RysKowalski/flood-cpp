#pragma once

#include "Grid.hpp"

class FloodSim {
public:
  FloodSim(int width, int height);
  void tick() const;

private:
  int width;
  int height;
  Grid grid;
};
