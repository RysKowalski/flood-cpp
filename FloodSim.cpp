#include "FloodSim.hpp"
#include "Grid.hpp"
#include <array>
#include <cstddef>
#include <thread>
#include <vector>

FloodSim::FloodSim(const std::size_t width, const std::size_t height, Grid map)
    : width(width), height(height), current(map), next(map),
      pixels(width * height * 4), k(0.1), generatorPower(2),
      start_barrier(std::thread::hardware_concurrency() + 1),
      end_barrier(std::thread::hardware_concurrency() + 1) {

  for (int i = 0; i < width * height * 4; i++) {
    pixels[i] = 255;
  }

  const unsigned count = std::thread::hardware_concurrency();
  const int chunk = width / count;

  for (unsigned i = 0; i < count; ++i) {
    const int start = i * chunk;

    const int end = (i == count - 1) ? width : start + chunk;

    workers.emplace_back(&FloodSim::worker, this, start, end);
  }
}

FloodSim::~FloodSim() {
  running = false;

  start_barrier.arrive_and_wait();

  for (auto &worker : workers) {
    worker.join();
  }
}

void FloodSim::tick() {
  next.clear();

  start_barrier.arrive_and_wait();
  end_barrier.arrive_and_wait();

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      current.at(x, y).value += next.at(x, y).value;
    }
  }
}

void FloodSim::worker(int start_x, int end_x) {
  while (true) {
    start_barrier.arrive_and_wait();

    if (!running) {
      break;
    }

    for (int x = start_x; x < end_x; ++x) {
      for (int y = 0; y < height; ++y) {
        process_cell(x, y);
        process_special_cell(x, y);
      }
    }

    end_barrier.arrive_and_wait();
  }
}

void FloodSim::process_cell(int x, int y) {
  if (!floodable(x, y)) {
    return;
  }

  const double center = current.at(x, y).value;

  double sum = 0.0;
  int count = 0;

  constexpr std::array<std::pair<int, int>, 4> dirs{{
      {-1, 0},
      {1, 0},
      {0, -1},
      {0, 1},
  }};

  for (const auto &[dx, dy] : dirs) {
    const int nx = x + dx;
    const int ny = y + dy;

    if (!floodable(nx, ny)) {
      continue;
    }

    sum += current.at(nx, ny).value;
    ++count;
  }

  next.at(x, y).value = k * (sum - count * center);
}

bool FloodSim::floodable(int x, int y) const {
  if (x < 0 || x > width - 1 || y < 0 || y > height - 1)
    return false;

  const auto type = current.at(x, y).type;
  return type == CellType::NOTHING || type == CellType::GENERATOR ||
         type == CellType::VOID;
}

void FloodSim::process_special_cell(int x, int y) {
  Cell &cell = current.at(x, y);
  switch (cell.type) {
  case CellType::GENERATOR: {
    next.at(x, y).value += generatorPower;
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

std::vector<std::uint8_t> FloodSim::get_pixels() {
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      update_pixel(x, y);
    }
  }
  return pixels;
}

void FloodSim::update_pixel(int x, int y) {
  const std::size_t i = (y * width + x) * 4;

  CellType &ctype = current.at(x, y).type;

  if (ctype == CellType::NOTHING) {

    double &val = current.at(x, y).value;

    if (val < 1) {
      pixels[i + 0] = 0; // R
      pixels[i + 1] = 0; // G
      pixels[i + 2] = 0; // B
      return;
    } else if (val < 13) {
      pixels[i + 0] = 177; // R
      pixels[i + 1] = 198; // G
      pixels[i + 2] = 209; // B
      return;
    } else if (val < 30) {
      pixels[i + 0] = 138; // R
      pixels[i + 1] = 163; // G
      pixels[i + 2] = 244; // B
      return;
    } else if (val < 90) {
      pixels[i + 0] = 128; // R
      pixels[i + 1] = 117; // G
      pixels[i + 2] = 165; // B
      return;
    } else if (val < 180) {
      pixels[i + 0] = 239; // R
      pixels[i + 1] = 201; // G
      pixels[i + 2] = 152; // B
      return;
    } else if (val < 300) {
      pixels[i + 0] = 203; // R
      pixels[i + 1] = 216; // G
      pixels[i + 2] = 126; // B
      return;
    } else {
      pixels[i + 0] = 80;  // R
      pixels[i + 1] = 71;  // G
      pixels[i + 2] = 123; // B
      return;
    }
  } else if (ctype == CellType::WALL) {
    pixels[i + 0] = 255; // R
    pixels[i + 1] = 255; // G
    pixels[i + 2] = 0;   // B
    return;
  } else if (ctype == CellType::GENERATOR) {
    pixels[i + 0] = 255; // R
    pixels[i + 1] = 255; // G
    pixels[i + 2] = 255; // B
    return;
  } else if (ctype == CellType::VOID) {
    pixels[i + 0] = 255; // R
    pixels[i + 1] = 0;   // G
    pixels[i + 2] = 0;   // B
    return;
  }
}

void FloodSim::place_wall(int x, int y) {
  current.at(x, y).type = CellType::WALL;
}

void FloodSim::place_generator(int x, int y, double power) {
  Cell &cell = current.at(x, y);
  cell.type = CellType::GENERATOR;
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
