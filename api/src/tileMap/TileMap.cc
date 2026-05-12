//
// Created by petitfiloue on 8.05.2026.
//

#include <tileMap/TileMap.h>
#include <iostream>

TileMap::TileMap()
    : m_mapWidth(0)
    , m_mapHeight(0)
    , m_tileSize(0)
{
}

bool TileMap::load(const std::string& tilesetPath,
                   int tileSize,
                   int mapWidth,
                   int mapHeight)
{
    m_tileSize  = tileSize;
    m_mapWidth  = mapWidth;
    m_mapHeight = mapHeight;

    if (!m_tileSet.load(tilesetPath, sf::Vector2u(tileSize, tileSize)))
    {
        std::cerr << "[TileMap] Erreur : chargement du tileset échoué." << std::endl;
        return false;
    }

    // On crée un premier layer de base vide
    addLayer();

    return true;
}


void TileMap::addLayer()
{
    TileLayer layer;
    layer.init(m_mapWidth, m_mapHeight, m_tileSize);
    m_layers.push_back(layer);
}

void TileMap::removeLayer(int index)
{
    if (index < 0 || index >= static_cast<int>(m_layers.size()))
        return;

    m_layers.erase(m_layers.begin() + index);
}

int TileMap::getLayerCount() const
{
    return static_cast<int>(m_layers.size());
}


void TileMap::setTile(int layer, int x, int y, int tileID)
{
    if (!isValidPosition(layer, x, y))
        return;

    m_layers[layer].setTile(x, y, tileID, m_tileSet);
}

int TileMap::getTile(int layer, int x, int y) const
{
    if (!isValidPosition(layer, x, y))
        return -1;

    return m_layers[layer].getTile(x, y);
}

void TileMap::clearTile(int layer, int x, int y)
{
    if (!isValidPosition(layer, x, y))
        return;

    m_layers[layer].clearTile(x, y, m_tileSet);
}


void TileMap::setLayerVisible(int index, bool visible)
{
    if (index < 0 || index >= static_cast<int>(m_layers.size()))
        return;

    m_layers[index].setVisible(visible);
}

bool TileMap::isLayerVisible(int index) const
{
    if (index < 0 || index >= static_cast<int>(m_layers.size()))
        return false;

    return m_layers[index].isVisible();
}


sf::Vector2i TileMap::worldToTile(sf::Vector2f worldPos) const
{
    int tileX = static_cast<int>(worldPos.x) / m_tileSize;
    int tileY = static_cast<int>(worldPos.y) / m_tileSize;

    return sf::Vector2i(tileX, tileY);
}


void TileMap::draw(sf::RenderWindow& window) const
{
    for (const TileLayer& layer : m_layers)
        layer.draw(window, m_tileSet);
}


int TileMap::getTileSize()  const { return m_tileSize;  }
int TileMap::getMapWidth()  const { return m_mapWidth;  }
int TileMap::getMapHeight() const { return m_mapHeight; }


bool TileMap::isValidPosition(int layer, int x, int y) const
{
    if (layer < 0 || layer >= static_cast<int>(m_layers.size()))
    {
        std::cerr << "[TileMap] Layer invalide : " << layer << std::endl;
        return false;
    }
    if (x < 0 || x >= m_mapWidth || y < 0 || y >= m_mapHeight)
    {
        std::cerr << "[TileMap] Position invalide : (" << x << ", " << y << ")" << std::endl;
        return false;
    }
    return true;
}
