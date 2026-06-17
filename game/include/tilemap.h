//
// Created by sebas on 20.05.2026.
//

#ifndef CITYBUILDER_TILEMAP_H
#define CITYBUILDER_TILEMAP_H

#include <mdspan>
#include <vector> // Include for std::vector
#include <SFML/System/Vector2.hpp> // Include for sf::Vector2i
#include "FastNoiseLite.h"
#include "tiles/tile.h"
#include "game_types.h"
#include "graphics/tilemap_renderer.h"
#include "graphics/tilesheet.h"

class Tilemap {

    graphics::TilemapRenderer terrain_renderer_;
    graphics::Tilesheet<TerrainTiles> terrain_tilesheet_;

    graphics::TilemapRenderer ressources_renderer_;
    graphics::Tilesheet<RessourcesTiles> ressources_tilesheet_;




    std::vector<sf::Vector2i> grid_coordinates_; // Renamed from walkable_tile_positions_

public:
    sf::Vector2i gridSize_;// Added to store the grid dimensions

    std::vector<api::tiles::Tile<TerrainTiles> > terrain;
    std::vector<api::tiles::Tile<RessourcesTiles> > wood;
    std::vector<api::tiles::Tile<RessourcesTiles> > rock;

    std::mdspan<sf::Vector2i, std::dextents<std::size_t, 2>> GetWalkableTiles(); // Corrected return type
RessourcesTiles GetWoodTileType(sf::Vector2i pos)const;RessourcesTiles GetRockTileType(sf::Vector2i pos)const;

    // New public method to get the terrain tile type at a specific grid position
    TerrainTiles GetTerrainTileType(sf::Vector2i pos) const;

    void Setup(sf::Vector2i gridSize, sf::Vector2f gridOffset);
    void Draw(sf::RenderWindow &window);

};

#endif //CITYBUILDER_TILEMAP_H
