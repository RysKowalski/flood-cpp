#pragma once

#include "Grid.hpp"
#include <barrier>
#include <cstddef>
#include <cstdint>
#include <string>
#include <thread>

class FloodSim {
public:
  FloodSim(const std::size_t width, const std::size_t height, Grid map);
  ~FloodSim();
  void tick();
  std::vector<std::uint8_t> get_pixels();

  double k;
  double generatorPower;

  void place_wall(int x, int y);
  void place_generator(int x, int y, double power);
  void place_nothing(int x, int y);
  void place_void(int x, int y);
  void set_flood_value(int x, int y, double value);
  void change_flood_value(int x, int y, double difference);

  void save(std::string &path);

private:
  std::size_t width;
  std::size_t height;
  Grid current;
  Grid next;
  std::vector<std::uint8_t> pixels;

  void worker(int start_x, int end_x);
  bool running{true};
  std::barrier<> start_barrier;
  std::barrier<> end_barrier;
  std::vector<std::thread> workers;

  void flood_cell(int x, int y);
  void process_special_cell(int x, int y);
  bool floodable(int x, int y) const;
  void update_pixel(int x, int y);
  void process_cell(int x, int y);
};
