//
// Created by petitfiloue on 10.05.2026.
//

#ifndef CITYBUILDER_PERFCHECKER_H
#define CITYBUILDER_PERFCHECKER_H

#include <SFML/Graphics.hpp>

class FPSCounter
{
public:
  FPSCounter();

  void draw(sf::RenderWindow& window);
  void setPosition(float x, float y);
  void setColor(sf::Color color);
  void setSize(unsigned int size);

private:
  sf::Font  m_font;
  sf::Text  m_text;
  sf::Clock m_clock;
  float     m_fpsHistory[60] = {};
  int       m_index = 0;
};

#endif  // CITYBUILDER_PERFCHECKER_H
