#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

struct GeneratorData {
  double power;
};

enum class CellType { NOTHING, WALL, GENERATOR, VOID };

struct Cell {
  CellType type = CellType::NOTHING;
  double value = 0.0;
  static constexpr std::uint32_t invalid_index =
      std::numeric_limits<std::uint32_t>::max();
  std::uint32_t generator_index = invalid_index;
};

class Grid {
public:
  Grid(std::size_t width, std::size_t height);

  Cell &at(std::size_t x, std::size_t y);
  const Cell &at(std::size_t x, std::size_t y) const;
  void clear();

  std::size_t width() const;
  std::size_t height() const;

private:
  std::size_t width_;
  std::size_t height_;
  std::vector<Cell> cells_;
};
