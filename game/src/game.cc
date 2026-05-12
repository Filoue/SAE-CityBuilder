//
// Created by petitfiloue on 07.05.2026.
//

#include "game.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "PerfChecker/FPSCounter.h"
#include "tileMap/TileMap.h"
#include "tileMap/TileMapEditor.h"

namespace game {

sf::RenderWindow window;
TileMap tileMap;
TileSet tileSet;
TileMapEditor editor(tileMap);
FPSCounter fps;


void Setup() {
  window.create(sf::VideoMode::getDesktopMode(), "CityBuilder");
  window.setFramerateLimit(60);

 //if (!tileMap.load("asset/tilemap.png", 32, 25, 18)) {}


  if (!tileMap.load("assets/tilemap.png", 64, 2500, 2500))

  tileMap.addLayer();
  tileMap.addLayer();

    for (int x = 0; x < 100; ++x) {
      for (int y = 0; y < 100; ++y) {
        tileMap.setTile(0,x,y,y);
      }
    }



  editor.setActiveTile(3);
  editor.setActiveLayer(0);

 }

void Loop() {
  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }

      if (const auto* keypressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keypressed->code == sf::Keyboard::Key::Num1) editor.setActiveLayer(0);
        if (keypressed->code == sf::Keyboard::Key::Num2) editor.setActiveLayer(1);
        if (keypressed->code == sf::Keyboard::Key::Num3) editor.setActiveLayer(2);

        if (keypressed->code == sf::Keyboard::Key::P) {
          editor.setActiveTile(editor.getActiveTile() + 1);
        }
        if (keypressed->code == sf::Keyboard::Key::O) {
          editor.setActiveTile(editor.getActiveTile() - 1);
        }
      }

      if (const auto* mouseScroll = event->getIf<sf::Event::MouseButtonPressed>()) {

      }

      editor.handleEvent(*event, window);
    }

    window.clear();
    tileMap.draw(window);
    fps.draw(window);
    window.display();
  }
}
}  // namespace game
