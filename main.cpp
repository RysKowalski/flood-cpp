#include <SFML/Graphics.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/WindowEnums.hpp>
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
      "FloodSim", sf::Style::Default, sf::State::Windowed);

  window.setFramerateLimit(60);
  sf::View view(sf::FloatRect({0.f, 0.f}, {static_cast<float>(grid_width),
                                           static_cast<float>(grid_height)}));

  window.setView(view);
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
      if (event->is<sf::Event::Resized>()) {
        sf::Vector2u size = window.getSize();

        float windowRatio =
            static_cast<float>(size.x) / static_cast<float>(size.y);

        float viewWidth = static_cast<float>(grid_width);
        float viewHeight = static_cast<float>(grid_height);
        float viewRatio = viewWidth / viewHeight;

        sf::View view(sf::FloatRect(sf::Vector2f{0.f, 0.f},
                                    sf::Vector2f{viewWidth, viewHeight}));

        if (windowRatio > viewRatio) {
          float scaledWidth = viewRatio / windowRatio;

          view.setViewport(
              sf::FloatRect(sf::Vector2f{(1.f - scaledWidth) / 2.f, 0.f},
                            sf::Vector2f{scaledWidth, 1.f}));
        } else {
          float scaledHeight = windowRatio / viewRatio;

          view.setViewport(
              sf::FloatRect(sf::Vector2f{0.f, (1.f - scaledHeight) / 2.f},
                            sf::Vector2f{1.f, scaledHeight}));
        }

        window.setView(view);
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
        if (event->getIf<sf::Event::KeyPressed>()->code ==
            sf::Keyboard::Key::W) {
          sprite.move({0.f, 10.f});
        }
        if (event->getIf<sf::Event::KeyPressed>()->code ==
            sf::Keyboard::Key::A) {
          sprite.move({10.f, 0.f});
        }
        if (event->getIf<sf::Event::KeyPressed>()->code ==
            sf::Keyboard::Key::S) {
          sprite.move({0.f, -10.f});
        }
        if (event->getIf<sf::Event::KeyPressed>()->code ==
            sf::Keyboard::Key::D) {
          sprite.move({-10.f, 0.f});
        }
      }
      if (const auto *wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
        constexpr float zoomFactor = 1.1f;

        if (wheel->delta > 0)
          sprite.scale({1.f / zoomFactor, 1.f / zoomFactor}); // zoom in
        else
          sprite.scale({zoomFactor, zoomFactor}); // zoom out

        window.setView(view);
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
