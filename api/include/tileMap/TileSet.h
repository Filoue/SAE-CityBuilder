//
// Created by petitfiloue on 8.05.2026.
//

#ifndef CITYBUILDER_TILESET_H
#define CITYBUILDER_TILESET_H

#include <SFML/Graphics.hpp>
#include <string>
#include <optional>

class TileSet
{
public:
    TileSet();


    bool               load(const std::string& path, sf::Vector2u tileSize);
    int                getTileCount() const;
    sf::Vector2f       getTexCoords(int tileID) const;
    const sf::Texture& getTexture() const;
    sf::Vector2u       getTileSize() const;

private:
    sf::Texture  m_texture;
    sf::Vector2u m_tileSize;
    unsigned int m_columns;
    unsigned int m_rows;
};

#endif
