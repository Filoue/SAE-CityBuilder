//
// Created by petitfiloue on 10.05.2026.
//

#include <PerfChecker/FPSCounter.h>

#include <sstream>
#include <stdexcept>

FPSCounter::FPSCounter()
    : m_font()
    , m_text(m_font)   // sf::Text prend une font en SFML 3
    , m_clock()
    , m_index(0)
    , m_fpsHistory{}
{
  if (!m_font.openFromFile("assets/Nobile-Regular.ttf"))
    throw std::runtime_error("Impossible de charger la police arial.ttf");

  m_text.setCharacterSize(20);
  m_text.setFillColor(sf::Color::Yellow);
  m_text.setPosition({10.f, 10.f});
}

void FPSCounter::draw(sf::RenderWindow& window)
{
  float deltaTime = m_clock.restart().asSeconds();
  m_fpsHistory[m_index % 60] = 1.f / deltaTime;
  m_index++;

  float avg = 0;
  for (float f : m_fpsHistory) avg += f;
  avg /= 60.f;

  std::ostringstream ss;
  ss << "FPS: " << static_cast<int>(avg);
  m_text.setString(ss.str());

  window.draw(m_text);
}

void FPSCounter::setPosition(float x, float y)
{
  m_text.setPosition({x, y});  // Vector2f en SFML 3
}

void FPSCounter::setColor(sf::Color color)
{
  m_text.setFillColor(color);
}

void FPSCounter::setSize(unsigned int size)
{
  m_text.setCharacterSize(size);
}