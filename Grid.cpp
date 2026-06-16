#include "Grid.hpp"
#include <algorithm>
#include <fstream>
#include <ios>

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

void Grid::save(const std::string &path) const {
  std::ofstream file(path, std::ios::binary);

  if (!file) {
    throw std::runtime_error("failed to open file for writing");
  }

  file.write(reinterpret_cast<const char *>(&width_), sizeof(width_));

  file.write(reinterpret_cast<const char *>(&height_), sizeof(height_));

  const std::size_t count = cells_.size();

  file.write(reinterpret_cast<const char *>(&count), sizeof(count));

  file.write(reinterpret_cast<const char *>(cells_.data()),
             sizeof(Cell) * count);

  if (!file) {
    throw std::runtime_error("failed while writing file");
  }
}

Grid Grid::load(const std::string &path) {
  std::ifstream file(path, std::ios::binary);

  if (!file) {
    throw std::runtime_error("failed to open file for reading");
  }

  std::size_t width;
  std::size_t height;
  std::size_t count;

  file.read(reinterpret_cast<char *>(&width), sizeof(width));

  file.read(reinterpret_cast<char *>(&height), sizeof(height));

  file.read(reinterpret_cast<char *>(&count), sizeof(count));

  Grid grid(width, height);

  if (count != width * height) {
    throw std::runtime_error("invalid grid file");
  }

  file.read(reinterpret_cast<char *>(grid.cells_.data()), sizeof(Cell) * count);

  if (!file) {
    throw std::runtime_error("failed while reading file");
  }

  return grid;
}
