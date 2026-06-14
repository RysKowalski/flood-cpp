#include <SFML/Graphics.hpp>

#include <cstddef>
#include <vector>

#include "FloodSim.hpp"
#include "MindustryLoader.hpp"

int main() {
  MindustryLoader loader;
  Grid map{loader.load_map("mindustry-map.msav")};
  const std::size_t grid_width{map.width()};
  const std::size_t grid_height{map.height()};

  sf::RenderWindow window(
      sf::VideoMode(sf::Vector2u{static_cast<unsigned int>(grid_width),
                                 static_cast<unsigned int>(grid_height)}),
      "FloodSim");

  window.setFramerateLimit(0);

  FloodSim fsim(grid_width, grid_height, map);

  sf::Texture texture(sf::Vector2u{static_cast<unsigned int>(grid_width),
                                   static_cast<unsigned int>(grid_height)});
  sf::Sprite sprite(texture);

  int ticks_per_frame{4};
  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }

    for (int i{0}; i < ticks_per_frame; i++) {
      fsim.tick();
    }

    texture.update(fsim.get_pixels().data());

    window.clear();
    window.draw(sprite);
    window.display();
  }
}
