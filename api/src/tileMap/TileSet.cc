//
// Created by petitfiloue on 8.05.2026.
//

#include <tileMap/TileSet.h>
#include <iostream>

TileSet::TileSet()
    : m_tileSize(0u, 0u)
    , m_columns(0)
    , m_rows(0)
{
}

bool TileSet::load(const std::string& path, sf::Vector2u tileSize)
{
    auto texture = sf::Texture();
    auto result = texture.loadFromFile(path);
    if (!result)
    {
        std::cerr << "[TileSet] Erreur : impossible de charger " << path << std::endl;
        return false;
    }

    m_texture  = std::move(texture);
    m_tileSize = tileSize;
    m_columns  = m_texture.getSize().x / tileSize.x;
    m_rows     = m_texture.getSize().y / tileSize.y;

    return true;
}

int TileSet::getTileCount() const
{
    return static_cast<int>(m_columns * m_rows);
}

sf::Vector2f TileSet::getTexCoords(int tileID) const
{
    int col = tileID % static_cast<int>(m_columns);
    int row = tileID / static_cast<int>(m_columns);

    return sf::Vector2f(
        static_cast<float>(col * m_tileSize.x),
        static_cast<float>(row * m_tileSize.y)
    );
}

const sf::Texture& TileSet::getTexture() const
{
    return m_texture;
}

sf::Vector2u TileSet::getTileSize() const
{
    return m_tileSize;
}
