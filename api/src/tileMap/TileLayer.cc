//
// Created by petitfiloue on 8.05.2026.
//

#include <tileMap/TileLayer.h>

TileLayer::TileLayer()
    : m_width(0)
    , m_height(0)
    , m_tileSize(0)
    , m_visible(true)
{
}

void TileLayer::init(int width, int height, int tileSize)
{
    m_width    = width;
    m_height   = height;
    m_tileSize = tileSize;

    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_vertices.resize(static_cast<std::size_t>(width * height * 6));

    // Toutes les cases commencent vides (-1)
    m_tileIDs.assign(static_cast<std::size_t>(width * height), -1);
}

void TileLayer::setTile(int x, int y, int tileID, const TileSet& tileSet)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return;

    m_tileIDs[static_cast<std::size_t>(x + y * m_width)] = tileID;
    updateVertex(x, y, tileID, tileSet);
}

int TileLayer::getTile(int x, int y) const
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return -1;

    return m_tileIDs[static_cast<std::size_t>(x + y * m_width)];
}

void TileLayer::clearTile(int x, int y, const TileSet& tileSet)
{
    setTile(x, y, -1, tileSet);
}

void TileLayer::rebuild(const TileSet& tileSet)
{
    for (int y = 0; y < m_height; ++y)
        for (int x = 0; x < m_width; ++x)
            updateVertex(x, y, m_tileIDs[static_cast<std::size_t>(x + y * m_width)], tileSet);
}

void TileLayer::setVisible(bool visible)
{
    m_visible = visible;
}

bool TileLayer::isVisible() const
{
    return m_visible;
}

void TileLayer::draw(sf::RenderTarget& target, const TileSet& tileSet) const
{
    if (!m_visible)
        return;

    sf::RenderStates states(&tileSet.getTexture());
    target.draw(m_vertices, states);
}


void TileLayer::updateVertex(int x, int y, int tileID, const TileSet& tileSet)
{

    std::size_t index = static_cast<std::size_t>((x + y * m_width) * 6);

    if (tileID < 0)
    {
        for (std::size_t i = 0; i < 6; ++i)
        {
            m_vertices[index + i].position  = sf::Vector2f(0.f, 0.f);
            m_vertices[index + i].texCoords = sf::Vector2f(0.f, 0.f);
            m_vertices[index + i].color     = sf::Color::Transparent;
        }
        return;
    }

    float ts = static_cast<float>(m_tileSize);

    sf::Vector2f V0(  x       * ts,  y       * ts);
    sf::Vector2f V1( (x + 1)  * ts,  y       * ts);
    sf::Vector2f V2( (x + 1)  * ts, (y + 1)  * ts);
    sf::Vector2f V3(  x       * ts, (y + 1)  * ts);

    sf::Vector2f texOrigin = tileSet.getTexCoords(tileID);
    float tw = static_cast<float>(tileSet.getTileSize().x);
    float th = static_cast<float>(tileSet.getTileSize().y);

    sf::Vector2f T0(texOrigin.x,       texOrigin.y);
    sf::Vector2f T1(texOrigin.x + tw,  texOrigin.y);
    sf::Vector2f T2(texOrigin.x + tw,  texOrigin.y + th);
    sf::Vector2f T3(texOrigin.x,       texOrigin.y + th);

    m_vertices[index + 0] = {V0, sf::Color::White, T0};
    m_vertices[index + 1] = {V1, sf::Color::White, T1};
    m_vertices[index + 2] = {V2, sf::Color::White, T2};

    m_vertices[index + 3] = {V0, sf::Color::White, T0};
    m_vertices[index + 4] = {V2, sf::Color::White, T2};
    m_vertices[index + 5] = {V3, sf::Color::White, T3};
}
