#include "FloodSim.hpp"
#include "Grid.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>

FloodSim::FloodSim(const int width, const int height)
    : width(width), height(height), current(width, height), next(width, height),
      pixels(static_cast<std::size_t>(width) *
             static_cast<std::size_t>(height) * 4) {}

// void FloodSim::tick() {
//   constexpr double k{0.1};
//   constexpr std::array<std::pair<int, int>, 4> directions{{
//       {-1, 0},
//       {1, 0},
//       {0, -1},
//       {0, 1},
//   }};
//
//   for (int x{0}; x < width; x++) {
//     for (int y{0}; y < height; y++) {
//       if (not floodable(x, y))
//         continue;
//
//       double &c_value = current.at(x, y).value;
//       if (c_value <= 0) {
//         continue;
//       }
//
//       for (const auto &[dx, dy] : directions) {
//         int nx{x + dx};
//         int ny{y + dy};
//
//         if (not floodable(nx, ny))
//           continue;
//
//         double delta{current.at(nx, ny).value - c_value};
//         double flow{k * delta};
//
//         next.at(x, y).value += flow;
//         next.at(nx, ny).value -= flow;
//       }
//     }
//   }
//   next.at(50, 50).value += 4;
//   std::swap(current, next);
//   next.clear();
//   update_pixels();
// }

void FloodSim::tick() {
  constexpr double k{0.1};

  constexpr std::array<std::pair<int, int>, 4> directions{{
      {-1, 0},
      {1, 0},
      {0, -1},
      {0, 1},
  }};

  // separate accumulation buffer
  auto delta = next;
  delta.clear();

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      if (!floodable(x, y)) {
        continue;
      }

      const double c_value = current.at(x, y).value;
      if (c_value <= 0.0) {
        continue;
      }

      for (const auto &[dx, dy] : directions) {
        const int nx = x + dx;
        const int ny = y + dy;

        if (!floodable(nx, ny)) {
          continue;
        }

        const double n_value = current.at(nx, ny).value;

        const double flow = k * (n_value - c_value);

        delta.at(x, y).value += flow;
        delta.at(nx, ny).value -= flow;
      }
    }
  }

  // apply update once (synchronous step)
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      if (!floodable(x, y)) {
        continue;
      }

      current.at(x, y).value += delta.at(x, y).value;
    }
  }

  // injection (kept as in original, but applied to current state)
  current.at(50, 50).value += 20.0;

  update_pixels();
}

bool FloodSim::floodable(int x, int y) const {
  if (x < 0 || x > width - 1 || y < 0 || y > height - 1)
    return false;
  if (current.at(x, y).type == CellType::NOTHING) {
    return true;
  }
  return false;
}

void FloodSim::update_pixels() {

  for (int x{0}; x < width; x++) {
    for (int y{0}; y < height; y++) {
      const std::size_t i = (y * width + x) * 4;
      std::uint8_t blue = static_cast<std::uint8_t>(
          255 * std::clamp(current.at(x, y).value / 5.0, 0.0, 1.0));

      pixels[i + 0] = 0;    // R
      pixels[i + 1] = 0;    // G
      pixels[i + 2] = blue; // B
      pixels[i + 3] = 255;  // A
    }
  }
}
