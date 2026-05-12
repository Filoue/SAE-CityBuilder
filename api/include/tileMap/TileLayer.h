//
// Created by petitfiloue on 8.05.2026.
//


#ifndef CITYBUILDER_TILELAYER_H
#define CITYBUILDER_TILELAYER_H


#include <SFML/Graphics.hpp>
#include <vector>
#include <tileMap/TileSet.h>

class TileLayer
{
public:
    TileLayer();

    void init(int width, int height, int tileSize);

    void setTile(int x, int y, int tileID, const TileSet& tileSet);

    int  getTile(int x, int y) const;

    void clearTile(int x, int y, const TileSet& tileSet);

    void rebuild(const TileSet& tileSet);

    void setVisible(bool visible);
    bool isVisible() const;

    void draw(sf::RenderTarget& target, const TileSet& tileSet) const;

private:

    void updateVertex(int x, int y, int tileID, const TileSet& tileSet);

private:
    sf::VertexArray  m_vertices;
    std::vector<int> m_tileIDs;
    int              m_width;
    int              m_height;
    int              m_tileSize;
    bool             m_visible;
};

#endif
