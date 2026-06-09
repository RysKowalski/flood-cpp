#include <SFML/Graphics.hpp>

#include <vector>

#include "FloodSim.hpp"

int main() {
  constexpr unsigned grid_width{1500};
  constexpr unsigned grid_height{1500};

  sf::RenderWindow window(sf::VideoMode({grid_width, grid_height}), "FloodSim");

  window.setFramerateLimit(0);

  FloodSim fsim(grid_width, grid_height);
  fsim.place_generator(0, 0, 1000.0);

  for (int i{0}; i < 50; i += 2)
    fsim.place_void(20, i);

  sf::Texture texture(sf::Vector2u{grid_width, grid_height});
  sf::Sprite sprite(texture);

  int ticks_per_frame{8};
  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }

    for (int i{0}; i < ticks_per_frame; i++) {
      fsim.tick();
    }

    texture.update(fsim.pixels.data());

    window.clear();
    window.draw(sprite);
    window.display();
  }
}
