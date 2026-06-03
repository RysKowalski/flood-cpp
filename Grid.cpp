#include "Grid.hpp"
#include <algorithm>

Grid::Grid(std::size_t width, std::size_t height)
    : width_(width), height_(height), cells_(width * height) {}

Cell &Grid::at(std::size_t x, std::size_t y) { return cells_[y * width_ + x]; }

const Cell &Grid::at(std::size_t x, std::size_t y) const {
  return cells_[y * width_ + x];
}

std::size_t Grid::width() const { return width_; }

std::size_t Grid::height() const { return height_; }

void Grid::clear() {
  std::for_each(cells_.begin(), cells_.end(),
                [](Cell &cell) { cell.value = 0.0; });
}
