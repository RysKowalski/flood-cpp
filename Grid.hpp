#pragma once

#include <cstddef>
#include <string>
#include <vector>

enum class CellType { NOTHING, WALL, GENERATOR, VOID };

struct Cell {
  CellType type = CellType::NOTHING;
  double value = 0.0;
};

class Grid {
public:
  Grid(std::size_t width, std::size_t height);

  Cell &at(std::size_t x, std::size_t y);
  const Cell &at(std::size_t x, std::size_t y) const;
  void clear();

  std::size_t width() const;
  std::size_t height() const;

  void save(const std::string &path) const;
  static Grid load(const std::string &path);

private:
  std::size_t width_;
  std::size_t height_;
  std::vector<Cell> cells_;
};
