#include "FloodSim.hpp"
#include "Grid.hpp"
#include <array>

FloodSim::FloodSim(const int width, const int height)
    : width(width), height(height), current(width, height), next(width, height),
      pixels(static_cast<std::size_t>(width) *
             static_cast<std::size_t>(height) * 4) {}

void FloodSim::tick() {

  next.clear();

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      process_cell(x, y);
    }
  }

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      process_special_cell(x, y);
      current.at(x, y).value += next.at(x, y).value;
    }
  }

  update_pixels();
}

void FloodSim::process_cell(int x, int y) {
  if (floodable(x, y)) {
    flood_cell(x, y);
  }
}

bool FloodSim::floodable(int x, int y) const {
  if (x < 0 || x > width - 1 || y < 0 || y > height - 1)
    return false;

  const auto type = current.at(x, y).type;
  return type == CellType::NOTHING || type == CellType::GENERATOR ||
         type == CellType::VOID;
}

void FloodSim::process_special_cell(int x, int y) {
  switch (current.at(x, y).type) {
  case CellType::GENERATOR: {
    next.at(x, y).value += 30;
    break;
  }

  case CellType::VOID: {
    next.at(x, y).value = 0;
    break;
  }

  default: {
  }
  }
}

void FloodSim::flood_cell(int x, int y) {
  constexpr double k{0.1};

  constexpr std::array<std::pair<int, int>, 4> directions{{
      {-1, 0},
      {1, 0},
      {0, -1},
      {0, 1},
  }};

  const double c_value = current.at(x, y).value;
  if (c_value <= 0.0) {
    return;
  }

  for (const auto &[dx, dy] : directions) {
    const int nx = x + dx;
    const int ny = y + dy;

    if (!floodable(nx, ny)) {
      continue;
    }

    const double n_value = current.at(nx, ny).value;

    const double flow = k * (n_value - c_value);

    next.at(x, y).value += flow;
    next.at(nx, ny).value -= flow;
  }
}

void FloodSim::update_pixels() {
  for (int x{0}; x < width; x++) {
    for (int y{0}; y < height; y++) {
      update_pixel(x, y);
    }
  }
}

void FloodSim::update_pixel(int x, int y) {
  const std::size_t i = (y * width + x) * 4;

  CellType &ctype = current.at(x, y).type;

  if (ctype == CellType::NOTHING) {

    double &val = current.at(x, y).value;

    if (val < 1) {
      pixels[i + 0] = 0;   // R
      pixels[i + 1] = 0;   // G
      pixels[i + 2] = 0;   // B
      pixels[i + 3] = 255; // A
      return;
    } else if (val < 10) {
      pixels[i + 0] = 177; // R
      pixels[i + 1] = 198; // G
      pixels[i + 2] = 209; // B
      pixels[i + 3] = 255; // A
      return;
    } else if (val < 25) {
      pixels[i + 0] = 138; // R
      pixels[i + 1] = 163; // G
      pixels[i + 2] = 244; // B
      pixels[i + 3] = 255; // A
      return;
    } else if (val < 75) {
      pixels[i + 0] = 128; // R
      pixels[i + 1] = 117; // G
      pixels[i + 2] = 165; // B
      pixels[i + 3] = 255; // A
      return;
    } else if (val < 150) {
      pixels[i + 0] = 239; // R
      pixels[i + 1] = 201; // G
      pixels[i + 2] = 152; // B
      pixels[i + 3] = 255; // A
      return;
    } else if (val < 250) {
      pixels[i + 0] = 203; // R
      pixels[i + 1] = 216; // G
      pixels[i + 2] = 126; // B
      pixels[i + 3] = 255; // A
      return;
    } else {
      pixels[i + 0] = 80;  // R
      pixels[i + 1] = 71;  // G
      pixels[i + 2] = 123; // B
      pixels[i + 3] = 255; // A
      return;
    }
  } else if (ctype == CellType::WALL) {
    pixels[i + 0] = 255; // R
    pixels[i + 1] = 255; // G
    pixels[i + 2] = 0;   // B
    pixels[i + 3] = 255; // A
    return;
  } else if (ctype == CellType::GENERATOR) {
    pixels[i + 0] = 255; // R
    pixels[i + 1] = 255; // G
    pixels[i + 2] = 255; // B
    pixels[i + 3] = 255; // A
    return;
  } else if (ctype == CellType::VOID) {
    pixels[i + 0] = 255; // R
    pixels[i + 1] = 0;   // G
    pixels[i + 2] = 0;   // B
    pixels[i + 3] = 255; // A
    return;
  }
}

void FloodSim::place_wall(int x, int y) {
  current.at(x, y).type = CellType::WALL;
}

void FloodSim::place_generator(int x, int y, double power) {
  current.at(x, y).type = CellType::GENERATOR;
}

void FloodSim::place_nothing(int x, int y) {
  current.at(x, y).type = CellType::NOTHING;
}

void FloodSim::place_void(int x, int y) {
  current.at(x, y).type = CellType::VOID;
}

void FloodSim::set_flood_value(int x, int y, double value) {
  current.at(x, y).value = value;
}

void FloodSim::change_flood_value(int x, int y, double difference) {
  current.at(x, y).value += difference;
}
