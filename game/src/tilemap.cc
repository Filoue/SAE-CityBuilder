//
// Created by sebas on 20.05.2026.
//

#include "tilemap.h"
#include <algorithm>
#include <print>

#include "game_types.h"
#include "tiles/tilemap_generator.h"

void Tilemap::Setup(sf::Vector2i gridSize, sf::Vector2f gridOffset){
    gridSize_ = gridSize; // Initialize gridSize_
    // On passe gridSize et la taille de la tuile (32, 32)
    terrain = tiles::generator::GenerateTerrain(gridSize, {32.f, 32.f});

    // Sécurité supplémentaire : on s'assure que l'ordre est correct pour GetTerrainTileType
    std::sort(terrain.begin(), terrain.end(), [](const auto& a, const auto& b) {
        if (a.pos.y != b.pos.y) return a.pos.y < b.pos.y;
        return a.pos.x < b.pos.x;
    });

    wood = tiles::generator::SeedAndGrow(terrain, RessourcesTiles::kWood, TerrainTiles::kGrassB);
    rock = tiles::generator::SeedAndGrow(terrain, RessourcesTiles::kRock, TerrainTiles::kSandA);

    // Populate grid_coordinates_ with all grid positions
    grid_coordinates_.clear();
    grid_coordinates_.reserve(gridSize_.x * gridSize_.y);
    for (int y = 0; y < gridSize_.y; ++y) { // Cast to int
        for (int x = 0; x < gridSize_.x; ++x) { // Cast to int
            grid_coordinates_.emplace_back(x, y);
        }
    }

    if (terrain_tilesheet_.InitTileSheet("_assets/tiles/RTS_medieval@2_no_margins_transparent.png", 128)) {
        terrain_tilesheet_.AddTile(TerrainTiles::kGrassA, 0, 0);
        terrain_tilesheet_.AddTile(TerrainTiles::kGrassB, 1, 0);
        terrain_tilesheet_.AddTile(TerrainTiles::kWaterA, 0, 2);
        terrain_tilesheet_.AddTile(TerrainTiles::kWaterB, 1, 2);
        terrain_tilesheet_.AddTile(TerrainTiles::kForest, 3, 3);
        terrain_tilesheet_.AddTile(TerrainTiles::kSandA, 3, 0);
        terrain_tilesheet_.AddTile(TerrainTiles::kSandB, 3, 1);




        // init textures -------------------------------------------------------------------
        terrain_renderer_.SetTexture(terrain_tilesheet_.GetTexture());
        terrain_renderer_.ClearVertices();

        for (auto &tile: terrain) {
            terrain_renderer_.AddTile(tile.pos, gridOffset, terrain_tilesheet_.GetBounds(tile.type));
        }
    } else {
        std::println("ERREUR : Impossible de charger la texture du terrain ! Vérifiez le chemin et le répertoire de travail.");
    }

    if (ressources_tilesheet_.InitTileSheet("_assets/tiles/RTS_medieval@2_no_margins_transparent.png", 128)) {
        ressources_tilesheet_.AddTile(RessourcesTiles::kWood, 3, 3);
        ressources_tilesheet_.AddTile(RessourcesTiles::kRock, 5, 4);
        ressources_tilesheet_.AddTile(RessourcesTiles::kFood, 5, 5);


        // init textures -------------------------------------------------------------------
        ressources_renderer_.SetTexture(ressources_tilesheet_.GetTexture());
        ressources_renderer_.ClearVertices();

        for (auto &tile: wood) {
            ressources_renderer_.AddTile(tile.pos, gridOffset, ressources_tilesheet_.GetBounds(tile.type));
        }
        for (auto &tile: rock) {
            ressources_renderer_.AddTile(tile.pos, gridOffset, ressources_tilesheet_.GetBounds(tile.type));
        }
    } else {
        std::println("ERREUR : Impossible de charger la texture des ressources ! Vérifiez le chemin et le répertoire de travail.");
    }
}

void Tilemap::Draw(sf::RenderWindow &window){
    terrain_renderer_.Draw(window);
    ressources_renderer_.Draw(window);
}

std::mdspan<sf::Vector2i, std::dextents<std::size_t, 2>> Tilemap::GetWalkableTiles(){
    return std::mdspan(grid_coordinates_.data(), static_cast<size_t>(gridSize_.y), static_cast<size_t>(gridSize_.x));
}

RessourcesTiles Tilemap::GetWoodTileType(sf::Vector2i pos) const {
    if (pos.x < 0 || pos.x >= gridSize_.x || pos.y < 0 || pos.y >= gridSize_.y) {
        return RessourcesTiles::kWood;
    }

    size_t index = pos.y * gridSize_.x + pos.x;

    if (index >= wood.size()) {
        return RessourcesTiles::kWood;
    }

    return wood[index].type;
}

RessourcesTiles Tilemap::GetRockTileType(sf::Vector2i pos) const {
    if (pos.x < 0 || pos.x >= gridSize_.x || pos.y < 0 || pos.y >= gridSize_.y) {
        return RessourcesTiles::kRock;
    }

    size_t index = pos.y * gridSize_.x + pos.x;

    if (index >= rock.size()) {
        return RessourcesTiles::kRock;
    }

    return rock[index].type;
}

TerrainTiles Tilemap::GetTerrainTileType(sf::Vector2i pos) const {
    // Perform bounds checking
    if (pos.x < 0 || pos.x >=gridSize_.x || // Cast to int
        pos.y < 0 || pos.y >= gridSize_.y) { // Cast to int
        // Return a default non-walkable type or throw an error
        // For simplicity, returning a non-walkable type (e.g., kWaterA)
        return TerrainTiles::kWaterA;
    }

    // Calculate the 1D index from 2D coordinates (assuming row-major order)
    size_t index = pos.y * gridSize_.x + pos.x; // Cast to int

    // Ensure index is within bounds of the terrain vector
    if (index >= terrain.size()) {
        // This case should ideally not be reached if gridSize_ and terrain.size() are consistent
        return TerrainTiles::kWaterA; // Fallback
    }

    return terrain[index].type;
}
