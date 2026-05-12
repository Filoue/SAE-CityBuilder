//
// Created by petitfiloue on 8.05.2026.
//

#ifndef CITYBUILDER_TILEMAPEDITOR_H
#define CITYBUILDER_TILEMAPEDITOR_H

#include <SFML/Graphics.hpp>
#include <tileMap/TileMap.h>

class TileMapEditor
{
public:
    TileMapEditor(TileMap& tileMap);

    void handleEvent(const sf::Event& event, sf::RenderWindow& window);

    void setActiveTile(int tileID);
    void setActiveLayer(int layerIndex);

    int getActiveTile()  const;
    int getActiveLayer() const;

    void paint(sf::Vector2i tilePos);
    void erase(sf::Vector2i tilePos);

private:
    sf::Vector2i getMouseTilePos(sf::RenderWindow& window) const;

private:
    TileMap& m_tileMap;
    int      m_activeTileID;
    int      m_activeLayer;
};

#endif
