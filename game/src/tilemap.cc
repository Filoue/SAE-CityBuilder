//
// Created by sebas on 20.05.2026.
//

#include "tilemap.h"

#include <expected>
#include <print>

#include "game_types.h"
#include "tiles/tilemap_generator.h"

void Tilemap::Setup(sf::Vector2i gridSize, sf::Vector2f gridOffset){
    // On passe gridSize et la taille de la tuile (32, 32)
    terrain = tiles::generator::GenerateTerrain();

    ressources = tiles::generator::SeedAndGrow(terrain);

    // Populate grid_coordinates_ with all grid positions
    grid_coordinates_.clear();
    grid_coordinates_.reserve(api::tiles::WorldSettings::nb_tiles.x * api::tiles::WorldSettings::nb_tiles.y);
    for (int y = 0; y < api::tiles::WorldSettings::nb_tiles.x; ++y) { // Cast to int
        for (int x = 0; x < api::tiles::WorldSettings::nb_tiles.x; ++x) { // Cast to int
            grid_coordinates_.emplace_back(x, y);
        }
    }

    if (terrain_tilesheet_.InitTileSheet("_assets/tiles/RTS_medieval@2_no_margins_transparent - Copie.png", 128)) {
        terrain_tilesheet_.AddTile(TerrainTiles::kGrassA, 0, 0);
        terrain_tilesheet_.AddTile(TerrainTiles::kGrassB, 1, 0);
        terrain_tilesheet_.AddTile(TerrainTiles::kWaterA, 0, 2);
        terrain_tilesheet_.AddTile(TerrainTiles::kWaterB, 1, 2);
        terrain_tilesheet_.AddTile(TerrainTiles::kForest, 3, 3);
        terrain_tilesheet_.AddTile(TerrainTiles::kSandA, 2, 0);
        terrain_tilesheet_.AddTile(TerrainTiles::kSandB, 3, 1);




        // init textures -------------------------------------------------------------------
        terrain_renderer_.SetTexture(terrain_tilesheet_.GetTexture());
        terrain_renderer_.ClearVertices();

        for (auto &tile: terrain) {
            terrain_renderer_.AddTile(tile.pos, gridOffset, terrain_tilesheet_.GetBounds(tile.type));
        }
    } else {
    }

    if (ressources_tilesheet_.InitTileSheet("_assets/tiles/RTS_medieval@2_no_margins_transparent - Copie.png", 128)) {
        ressources_tilesheet_.AddTile(RessourcesTiles::kWood, 4, 3);
        ressources_tilesheet_.AddTile(RessourcesTiles::kRock, 5, 4);
        ressources_tilesheet_.AddTile(RessourcesTiles::kFood, 5, 5);
        ressources_tilesheet_.AddTile(RessourcesTiles::kNone, 20, 20);


        // init textures -------------------------------------------------------------------
        ressources_renderer_.SetTexture(ressources_tilesheet_.GetTexture());
        ressources_renderer_.ClearVertices();

        for (auto &tile : ressources)
        {
            ressources_renderer_.AddTile(tile.pos, gridOffset, ressources_tilesheet_.GetBounds(tile.type));
        }

    } else {
    }

    if (house_tilesheet_.InitTileSheet("_assets/tiles/RTS_medieval@2_no_margins_transparent - Copie.png", 128)) {
        house_tilesheet_.AddTile(Housing::kHunterHouse, 2, 3);
        house_tilesheet_.AddTile(Housing::kMinerHouse, 1, 3);
        house_tilesheet_.AddTile(Housing::kWoodCutterHouse, 0, 3);
    }else {
    }
}

void Tilemap::Draw(sf::RenderWindow &window){
    terrain_renderer_.Draw(window);
    ressources_renderer_.Draw(window);
    house_renderer_.Draw(window);
}

std::mdspan<sf::Vector2i, std::dextents<std::size_t, 2>> Tilemap::GetWalkableTiles(){
    return std::mdspan(grid_coordinates_.data(), static_cast<size_t>(api::tiles::WorldSettings::nb_tiles.y), static_cast<size_t>(api::tiles::WorldSettings::nb_tiles.x));
}

TerrainTiles Tilemap::GetTerrainTileType(sf::Vector2i pos) const {
    // Perform bounds checking
    if (pos.x < 0 || pos.x >=api::tiles::WorldSettings::nb_tiles.x || // Cast to int
        pos.y < 0 || pos.y >= api::tiles::WorldSettings::nb_tiles.y) { // Cast to int
        // Return a default non-walkable type or throw an error
        // For simplicity, returning a non-walkable type (e.g., kWaterA)
        return TerrainTiles::kWaterA;
    }

    // Calculate the 1D index from 2D coordinates (assuming row-major order)
    size_t index = api::tiles::WorldSettings::TilePosToIdx(pos); // Cast to int

    // Ensure index is within bounds of the terrain vector
    if (index >= terrain.size()) {
        // This case should ideally not be reached if gridSize_ and terrain.size() are consistent
        return TerrainTiles::kWaterA; // Fallback
    }

    return terrain[index].type;
}
std::expected<RessourcesTiles, TilemapError> Tilemap::GetRessourcesTileType(sf::Vector2i pos) const {
    // Perform bounds checking
    // Calculate the 1D index from 2D coordinates (assuming row-major order)
    size_t index = api::tiles::WorldSettings::TilePosToIdx(pos);
    if (index >= ressources.size()) {
        return std::unexpected(TilemapError::kOutOfBounds);
    }
    std::println("Index = {}, position = {} {}", index, pos.x, pos.y);// Cast to int
    return ressources[index].type;
}
