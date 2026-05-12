//
// Created by petitfiloue on 8.05.2026.
//

#ifndef CITYBUILDER_TILEMAP_H
#define CITYBUILDER_TILEMAP_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <tileMap/TileSet.h>
#include <tileMap/TileLayer.h>

class TileMap
{
public:
    TileMap();

    bool load(const std::string& tilesetPath,
              int tileSize,
              int mapWidth,
              int mapHeight);

    void addLayer();
    void removeLayer(int index);
    int  getLayerCount() const;

    void setTile(int layer, int x, int y, int tileID);
    int  getTile(int layer, int x, int y) const;
    void clearTile(int layer, int x, int y);

    void setLayerVisible(int index, bool visible);
    bool isLayerVisible(int index) const;

    sf::Vector2i worldToTile(sf::Vector2f worldPos) const;

    void draw(sf::RenderWindow& window) const;

    int getTileSize()  const;
    int getMapWidth()  const;
    int getMapHeight() const;

private:
    bool isValidPosition(int layer, int x, int y) const;

private:
    TileSet              m_tileSet;
    std::vector<TileLayer> m_layers;
    int                  m_mapWidth;
    int                  m_mapHeight;
    int                  m_tileSize;
};

#endif
