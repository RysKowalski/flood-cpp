#include "FloodSim.hpp"
#include "Grid.hpp"
#include <array>

FloodSim::FloodSim(const int width, const int height)
    : width(width), height(height), current(width, height),
      next(width, height) {}

void FloodSim::tick() {
  constexpr double k{0.1};
  constexpr std::array<std::pair<int, int>, 4> directions{{
      {-1, 0},
      {1, 0},
      {0, -1},
      {0, 1},
  }};

  for (int x{0}; x < width; x++) {
    for (int y{0}; y < height; y++) {
      if (not floodable(x, y))
        continue;

      double &c_value = current.at(x, y).value;
      if (c_value < 1) {
        continue;
      }

      for (const auto &[dx, dy] : directions) {
        int nx{x + dx};
        int ny{y + dy};
        if (not floodable(nx, ny))
          continue;

        double delta{current.at(nx, ny).value - c_value};
        double flow{delta * k};

        c_value += flow;
        next.at(nx, ny).value -= flow;
      }
    }
  }
  std::swap(current, next);
}

bool FloodSim::floodable(int x, int y) const {
  if (x < 0 | x)

    if (current.at(x, y).type == CellType::NOTHING) {
      return true;
    }
  return false;
}
