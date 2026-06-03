#include "FloodSim.hpp"
#include "Grid.hpp"

FloodSim::FloodSim(const int width, const int height)
    : width(width), height(height), current(width, height),
      next(width, height) {}

void FloodSim::tick() {
  for (int x{0}; x < width; x++) {
    for (int y{0}; y < height; y++) {
      if (floodable(x, y)) {
      }
    }
  }
}

bool FloodSim::floodable(int x, int y) const {
  if (current.at(x, y).type == CellType::NOTHING) {
    return true;
  }
  return false;
}
