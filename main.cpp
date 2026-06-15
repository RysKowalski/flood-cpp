#include <SFML/Graphics.hpp>

#include <cstddef>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "FloodSim.hpp"
#include "MindustryLoader.hpp"

sf::Text get_number_text(sf::Font &font, sf::Vector2f position,
                         std::string default_text) {
  sf::Text text(font);
  text.setFillColor(sf::Color::White);
  text.setCharacterSize(15);
  text.setStyle(sf::Text::Bold);
  text.setPosition(position);
  text.setString(default_text);
  return text;
}

sf::Text get_paused_text(sf::Font &font) {
  sf::Text text(font);
  text.setFillColor(sf::Color::Red);
  text.setCharacterSize(20);
  text.setStyle(sf::Text::Bold);
  text.setPosition({0.f, 62.f});
  text.setString("Paused!");
  return text;
}

std::string get_path(int argc, char *argv[]) {
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "-m" && i + 1 < argc) {
      return std::string(argv[i + 1]);
    }
  }
  std::cerr << "Error: missing -m <path to .msav>\n";
  std::exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  MindustryLoader loader(get_path(argc, argv));
  Grid map{loader.load_map()};

  const std::size_t grid_width = map.width();
  const std::size_t grid_height = map.height();

  sf::RenderWindow window(
      sf::VideoMode(sf::Vector2u{static_cast<unsigned int>(grid_width),
                                 static_cast<unsigned int>(grid_height)}),
      "FloodSim", sf::Style::Default, sf::State::Windowed);

  window.setFramerateLimit(60);
  sf::View uiView(
      sf::FloatRect({0.f, 0.f}, {static_cast<float>(window.getSize().x),
                                 static_cast<float>(window.getSize().y)}));
  FloodSim fsim(grid_width, grid_height, map);

  sf::Texture texture(sf::Vector2u{static_cast<unsigned int>(grid_width),
                                   static_cast<unsigned int>(grid_height)});

  sf::Sprite sprite(texture);

  sf::Font font("font.ttf");
  sf::Text paused_text = get_paused_text(font);
  sf::Text ticks_text = get_number_text(font, {0.f, 0.f}, "1");
  sf::Text skip_ticks_amount_text = get_number_text(font, {0.f, 16.f}, "1");
  sf::Text k_text = get_number_text(font, {0.f, 30.f}, "k: 0.1");
  sf::Text generator_power_text =
      get_number_text(font, {0.f, 46.f}, "generator power: 0.1");

  sf::View view(sf::FloatRect({0.f, 0.f}, {static_cast<float>(grid_width),
                                           static_cast<float>(grid_height)}));
  window.setView(view);

  sf::Vector2i last_mouse_pos{0, 0};
  bool dragging = false;

  int main_tick_count = 0;
  int skip_ticks_amount = 1;
  int ticks_per_frame = 1;
  bool paused = false;

  while (window.isOpen()) {
    main_tick_count++;
    while (const std::optional event = window.pollEvent()) {

      if (event->is<sf::Event::Closed>()) {
        window.close();
      }

      if (event->is<sf::Event::Resized>()) {
        sf::Vector2u size = window.getSize();

        float window_ratio =
            static_cast<float>(size.x) / static_cast<float>(size.y);

        float view_width = static_cast<float>(grid_width);
        float view_height = static_cast<float>(grid_height);

        float view_ratio = view_width / view_height;

        sf::FloatRect viewport;

        if (window_ratio > view_ratio) {
          float scale = view_ratio / window_ratio;
          viewport = sf::FloatRect({(1.f - scale) / 2.f, 0.f}, {scale, 1.f});
        } else {
          float scale = window_ratio / view_ratio;
          viewport = sf::FloatRect({0.f, (1.f - scale) / 2.f}, {1.f, scale});
        }

        view.setViewport(viewport);
        window.setView(view);

        uiView.setSize({static_cast<float>(window.getSize().x),
                        static_cast<float>(window.getSize().y)});
        uiView.setCenter(uiView.getSize() / 2.f);
      }

      if (event->is<sf::Event::KeyPressed>()) {
        auto key = event->getIf<sf::Event::KeyPressed>()->code;

        if (key == sf::Keyboard::Key::Space) {
          paused = !paused;
        }

        if (key == sf::Keyboard::Key::Up) {
          if (skip_ticks_amount > 1) {
            skip_ticks_amount--;
            skip_ticks_amount_text.setString(std::to_string(skip_ticks_amount));
          } else {
            ticks_per_frame++;
            ticks_text.setString(std::to_string(ticks_per_frame));
          }
        }
        if (key == sf::Keyboard::Key::Down) {
          if (ticks_per_frame > 1) {
            ticks_per_frame--;
            ticks_text.setString(std::to_string(ticks_per_frame));
          } else {
            skip_ticks_amount++;
            skip_ticks_amount_text.setString(std::to_string(skip_ticks_amount));
          }
        }

        if (key == sf::Keyboard::Key::K) {
          fsim.k += 0.01;
          std::string s = "";
          s.append("k: ").append(std::to_string(fsim.k));
          k_text.setString(s);
        }
        if (key == sf::Keyboard::Key::J) {
          fsim.k -= 0.01;
          std::string s = "";
          s.append("k: ").append(std::to_string(fsim.k));
          k_text.setString(s);
        }

        if (key == sf::Keyboard::Key::Left) {
          fsim.generatorPower -= 0.5;
          std::string s = "";
          s.append("generator power: ")
              .append(std::to_string(fsim.generatorPower));
          generator_power_text.setString(s);
        }
        if (key == sf::Keyboard::Key::Right) {
          fsim.generatorPower += 0.5;
          std::string s = "";
          s.append("generator power: ")
              .append(std::to_string(fsim.generatorPower));
          generator_power_text.setString(s);
        }

        if (key == sf::Keyboard::Key::W) {
          view.move({0.f, -10.f});
          window.setView(view);
        }
        if (key == sf::Keyboard::Key::S) {
          view.move({0.f, 10.f});
          window.setView(view);
        }
        if (key == sf::Keyboard::Key::A) {
          view.move({-10.f, 0.f});
          window.setView(view);
        }
        if (key == sf::Keyboard::Key::D) {
          view.move({10.f, 0.f});
          window.setView(view);
        }
      }

      if (const auto *wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {

        constexpr float zoom_factor = 1.1f;

        sf::Vector2i pixel = wheel->position;

        sf::Vector2f before = window.mapPixelToCoords(pixel, view);

        float factor = (wheel->delta > 0) ? (1.f / zoom_factor) : zoom_factor;

        view.zoom(factor);

        sf::Vector2f after = window.mapPixelToCoords(pixel, view);

        sf::Vector2f offset = before - after;

        view.move(offset);

        window.setView(view);
      }

      if (const auto *pressed = event->getIf<sf::Event::MouseButtonPressed>()) {

        if (pressed->button == sf::Mouse::Button::Right) {
          dragging = true;
          last_mouse_pos = pressed->position;
        }
      }

      if (const auto *released =
              event->getIf<sf::Event::MouseButtonReleased>()) {

        if (released->button == sf::Mouse::Button::Right) {
          dragging = false;
        }
      }

      if (const auto *moved = event->getIf<sf::Event::MouseMoved>()) {

        if (dragging) {
          sf::Vector2i delta = moved->position - last_mouse_pos;

          view.move(
              {-static_cast<float>(delta.x), -static_cast<float>(delta.y)});

          last_mouse_pos = moved->position;
          window.setView(view);
        }
      }
    }

    if (!paused) {
      if (main_tick_count % skip_ticks_amount == 0) {
        for (int i = 0; i < ticks_per_frame; ++i) {
          fsim.tick();
        }
      }
    }

    texture.update(fsim.get_pixels().data());

    window.clear();

    window.setView(view);
    window.draw(sprite);

    window.setView(uiView);
    if (paused)
      window.draw(paused_text);
    window.draw(ticks_text);
    window.draw(skip_ticks_amount_text);
    window.draw(k_text);
    window.draw(generator_power_text);
    window.display();
  }
}
