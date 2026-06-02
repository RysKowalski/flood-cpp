#include <SFML/Graphics.hpp>

#include <cstdint>
#include <random>
#include <vector>

#include "FloodSim.hpp"

class GameOfLife {
public:
  GameOfLife(unsigned width, unsigned height)
      : map_width(width), map_height(height),
        map_current(static_cast<std::size_t>(width) * height),
        map_next(static_cast<std::size_t>(width) * height) {}

  void randomize(double alive_probability = 0.5) {
    std::mt19937 rng{std::random_device{}()};
    std::bernoulli_distribution alive_dist{alive_probability};

    for (std::uint8_t &cell : map_current) {
      cell = alive_dist(rng);
    }
  }

  void update() {
    for (unsigned y{0}; y < map_height; ++y) {
      const unsigned y_up{(y == 0) ? (map_height - 1) : (y - 1)};
      const unsigned y_down{(y + 1 == map_height) ? 0 : (y + 1)};

      for (unsigned x{0}; x < map_width; ++x) {
        const unsigned x_left{(x == 0) ? (map_width - 1) : (x - 1)};
        const unsigned x_right{(x + 1 == map_width) ? 0 : (x + 1)};

        const std::size_t center{static_cast<std::size_t>(y) * map_width + x};

        const int neighbors{
            map_current[static_cast<std::size_t>(y_up) * map_width + x_left] +
            map_current[static_cast<std::size_t>(y_up) * map_width + x] +
            map_current[static_cast<std::size_t>(y_up) * map_width + x_right] +
            map_current[static_cast<std::size_t>(y) * map_width + x_left] +
            map_current[static_cast<std::size_t>(y) * map_width + x_right] +
            map_current[static_cast<std::size_t>(y_down) * map_width + x_left] +
            map_current[static_cast<std::size_t>(y_down) * map_width + x] +
            map_current[static_cast<std::size_t>(y_down) * map_width +
                        x_right]};

        const bool alive{map_current[center] != 0};

        map_next[center] = (alive && (neighbors == 2 || neighbors == 3)) ||
                           (!alive && neighbors == 3);
      }
    }

    map_current.swap(map_next);
  }

  [[nodiscard]] const std::vector<std::uint8_t> &cells() const noexcept {
    return map_current;
  }

  [[nodiscard]] unsigned width() const noexcept { return map_width; }

  [[nodiscard]] unsigned height() const noexcept { return map_height; }

private:
  unsigned map_width;
  unsigned map_height;

  std::vector<std::uint8_t> map_current;
  std::vector<std::uint8_t> map_next;
};

int main() {
  constexpr unsigned grid_width{1500};
  constexpr unsigned grid_height{1500};

  sf::RenderWindow window(sf::VideoMode({1200, 800}), "Game of Life 1500x1500");

  window.setFramerateLimit(60);

  GameOfLife game(grid_width, grid_height);
  game.randomize();

  FloodSim fsim(12);
  fsim.show();

  std::vector<std::uint8_t> pixels(static_cast<std::size_t>(grid_width) *
                                   grid_height * 4);

  sf::Texture texture({grid_width, grid_height});
  sf::Sprite sprite(texture);

  int ticks{0};
  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }

    ticks++;
    game.update();

    const auto &cells{game.cells()};

    for (std::size_t i{0}; i < cells.size(); ++i) {
      const std::size_t pixel_index{i * 4};

      const std::uint8_t value{static_cast<std::uint8_t>(cells[i] * 255)};

      pixels[pixel_index + 0] = (value * value * i * ticks) % 256;
      pixels[pixel_index + 1] = (value * value * i * ticks) % 256;
      pixels[pixel_index + 2] = (value * value * i * ticks) % 256;
      pixels[pixel_index + 3] = 255;
    }

    texture.update(pixels.data());

    window.clear();
    window.draw(sprite);
    window.display();
  }
}
