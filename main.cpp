#include <SFML/Graphics.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <string>
#include <vector>

#include "FloodSim.hpp"
#include "MindustryLoader.hpp"

int main() {
  MindustryLoader loader("mindustry-map.msav");
  Grid map{loader.load_map()};
  const std::size_t grid_width{map.width()};
  const std::size_t grid_height{map.height()};

  sf::RenderWindow window(
      sf::VideoMode(sf::Vector2u{static_cast<unsigned int>(grid_width),
                                 static_cast<unsigned int>(grid_height)}),
      "FloodSim");

  window.setFramerateLimit(60);
  FloodSim fsim(grid_width, grid_height, map);

  sf::Texture texture(sf::Vector2u{static_cast<unsigned int>(grid_width),
                                   static_cast<unsigned int>(grid_height)});
  sf::Sprite sprite(texture);

  sf::Font font("font.ttf");

  sf::Text paused_text(font);
  paused_text.setFillColor(sf::Color::Red);
  paused_text.setCharacterSize(20);
  paused_text.setStyle(sf::Text::Bold);
  paused_text.setPosition({0.f, 15.f});
  paused_text.setString("Paused!");

  sf::Text ticks_per_frame_text(font);
  ticks_per_frame_text.setFillColor(sf::Color::Black);
  ticks_per_frame_text.setCharacterSize(15);
  ticks_per_frame_text.setStyle(sf::Text::Bold);
  ticks_per_frame_text.setString("1");

  int ticks_per_frame{1};
  bool paused{false};
  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }

      if (event->is<sf::Event::KeyPressed>()) {
        if (event->getIf<sf::Event::KeyPressed>()->code ==
            sf::Keyboard::Key::Space) {
          paused = !paused;
        }
        if (event->getIf<sf::Event::KeyPressed>()->code ==
            sf::Keyboard::Key::Up) {
          ticks_per_frame++;
          ticks_per_frame_text.setString(std::to_string(ticks_per_frame));
        }
        if (event->getIf<sf::Event::KeyPressed>()->code ==
            sf::Keyboard::Key::Down) {
          if (ticks_per_frame > 1)
            ticks_per_frame--;
          ticks_per_frame_text.setString(std::to_string(ticks_per_frame));
        }
      }
    }

    if (not paused) {
      for (int i{0}; i < ticks_per_frame; i++) {
        fsim.tick();
      }
    }

    texture.update(fsim.get_pixels().data());

    window.clear();
    window.draw(sprite);
    if (paused)
      window.draw(paused_text);
    window.draw(ticks_per_frame_text);
    window.display();
  }
}
