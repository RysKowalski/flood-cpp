#include "FloodSim.hpp"
#include "Grid.hpp"
#include <iostream>

FloodSim::FloodSim(const int width, const int height)
    : width(width), height(height), grid(width, height) {}

void FloodSim::tick() const {
  std::cout << "value: " << height << " " << width << std::endl;
  std::cout << grid.height() << " " << grid.width() << std::endl;
}
