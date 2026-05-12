//
// Created by petitfiloue on 8.05.2026.
//

#include <tileMap/TileMapEditor.h>

TileMapEditor::TileMapEditor(TileMap& tileMap)
    : m_tileMap(tileMap)
    , m_activeTileID(0)
    , m_activeLayer(0)
{
}

void TileMapEditor::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{

    if (const auto* mouseBtn = event.getIf<sf::Event::MouseButtonPressed>())
    {
        sf::Vector2i tilePos = getMouseTilePos(window);

        // SFML 3 : sf::Mouse::Left → sf::Mouse::Button::Left
        if (mouseBtn->button == sf::Mouse::Button::Left)
            paint(tilePos);
        else if (mouseBtn->button == sf::Mouse::Button::Right)
            erase(tilePos);
    }

    if (event.is<sf::Event::MouseMoved>())
    {
        sf::Vector2i tilePos = getMouseTilePos(window);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
            paint(tilePos);
        else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
            erase(tilePos);
    }
}

void TileMapEditor::setActiveTile(int tileID)
{
    m_activeTileID = tileID;
}

void TileMapEditor::setActiveLayer(int layerIndex)
{
    m_activeLayer = layerIndex;
}

int TileMapEditor::getActiveTile() const
{
    return m_activeTileID;
}

int TileMapEditor::getActiveLayer() const
{
    return m_activeLayer;
}

void TileMapEditor::paint(sf::Vector2i tilePos)
{
    m_tileMap.setTile(m_activeLayer, tilePos.x, tilePos.y, m_activeTileID);
}

void TileMapEditor::erase(sf::Vector2i tilePos)
{
    m_tileMap.clearTile(m_activeLayer, tilePos.x, tilePos.y);
}


sf::Vector2i TileMapEditor::getMouseTilePos(sf::RenderWindow& window) const
{
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos   = window.mapPixelToCoords(mousePixel);

    return m_tileMap.worldToTile(worldPos);
}
