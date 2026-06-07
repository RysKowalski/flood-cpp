#include <SFML/Graphics.hpp>

#include <vector>

#include "FloodSim.hpp"

int main() {
  constexpr unsigned grid_width{100};
  constexpr unsigned grid_height{100};

  sf::RenderWindow window(sf::VideoMode({grid_width, grid_height}), "FloodSim");

  window.setFramerateLimit(60);

  FloodSim fsim(grid_width, grid_height);
  fsim.place_generator(50, 50, 1.0);

  sf::Texture texture(sf::Vector2u{grid_width, grid_height});
  sf::Sprite sprite(texture);

  int ticks{0};
  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }

    ticks++;
    fsim.tick();

    texture.update(fsim.pixels.data());

    window.clear();
    window.draw(sprite);
    window.display();
  }
}
