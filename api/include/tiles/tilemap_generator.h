//
// Created by sebas on 20.05.2026.
//

#ifndef CITYBUILDER_TILEMAP_GENERATOR_H
#define CITYBUILDER_TILEMAP_GENERATOR_H

#include <random>
#include <ranges>

#include "tile.h"
#include "tilemap.h"
#include "worldSettings/world_settings.h"

namespace tiles::generator {

    using namespace api::tiles;
    inline std::vector<Tile<TerrainTiles>> GenerateTerrain(){

        std::vector<Tile<TerrainTiles>> terrainMap;
        terrainMap.reserve(static_cast<size_t>(WorldSettings::nb_tiles.x) * WorldSettings::nb_tiles.y);

        FastNoiseLite noise;
        noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        noise.SetSeed(1337);
        noise.SetFrequency(0.0025f);

        FastNoiseLite cellularNoise;
        cellularNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
        cellularNoise.SetSeed(1337);
        cellularNoise.SetFrequency(0.0055f);

        cellularNoise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Euclidean);
        cellularNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
        cellularNoise.SetCellularJitter(1.f);

        cellularNoise.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2);
        cellularNoise.SetDomainWarpAmp(182.5f);

        cellularNoise.SetFractalType(FastNoiseLite::FractalType_DomainWarpProgressive);
        cellularNoise.SetFractalOctaves(3);
        cellularNoise.SetFractalLacunarity(2.18F);
        cellularNoise.SetFractalGain(0.01f);

        // FIX: Utilisation du Row-Major (Y puis X) pour correspondre à l'indexation y * width + x
        for (int y = 0; y < WorldSettings::nb_tiles.y; y++) {
            for (int x = 0; x < WorldSettings::nb_tiles.x; x++) {
                // Calcul direct de la position monde pour éviter les décalages flottants
                sf::Vector2f worldPos{static_cast<float>(x) * WorldSettings::tile_size.x, static_cast<float>(y) * WorldSettings::tile_size.y};

                // Generator stuff -----------------------------
                Biome b = Biome::kOcean;
                float cellularValue = std::abs(cellularNoise.GetNoise(worldPos.x, worldPos.y));


                if (cellularValue >= 0.66) {
                    b = Biome::kForest;
                }else if (cellularValue >= 0.33) {
                    b = Biome::kPlain;
                }else if (cellularValue >= 0.15) {
                    b = Biome::kOcean;
                }else if (cellularValue >= 0) {
                    b = Biome::kDesert;
                }

                switch (b) {
                    case Biome::kOcean:
                        terrainMap.emplace_back(Tile{worldPos, TerrainTiles::kWaterA, false});
                        break;
                    case Biome::kPlain:
                        terrainMap.emplace_back(Tile{worldPos, TerrainTiles::kGrassA, true});
                        break;
                    case Biome::kForest:
                        terrainMap.emplace_back(Tile{worldPos, TerrainTiles::kGrassB, true});
                        break;
                    case Biome::kDesert:
                        terrainMap.emplace_back(Tile{worldPos, TerrainTiles::kSandA, true});
                        break;
                }
            }
        }
        return terrainMap;
    }

    inline std::vector<Tile<RessourcesTiles>> SeedAndGrow(const std::vector<Tile<TerrainTiles>>& terrain){

        std::vector<Tile<RessourcesTiles>> ressourceMap;

        FastNoiseLite ressourceNoise;
        ressourceNoise.SetSeed(1337);
        ressourceNoise.SetFrequency(0.9);



        for (auto y = 0; WorldSettings::nb_tiles.y > y; y++) {
            for (auto x = 0; WorldSettings::nb_tiles.x > x; x++) {
                size_t index = WorldSettings::TilePosToIdx(sf::Vector2i{x, y});
                sf::Vector2f worldPos{static_cast<float>(x) * WorldSettings::tile_size.x, static_cast<float>(y) * WorldSettings::tile_size.y};
                float ressourceNoiseValue = std::abs(ressourceNoise.GetNoise(worldPos.x, worldPos.y));

                RessourcesTiles ressource = RessourcesTiles::kNone;
                bool walkable = true;

                if (ressourceNoiseValue >= 0.6f) {
                    TerrainTiles current_terrain = terrain.at(index).type;
                    if (current_terrain == TerrainTiles::kGrassB) {
                        ressource = RessourcesTiles::kWood;
                        walkable = false;
                    } else if (current_terrain == TerrainTiles::kSandA || current_terrain == TerrainTiles::kSandB) {
                        ressource = RessourcesTiles::kRock;
                        walkable = false;
                    }
                }
                ressourceMap.emplace_back(worldPos, ressource, walkable);
            }
        }

        return ressourceMap;

    }

    inline std::vector<Tile<Housing>> PlaceAHouse(Housing h) {
        std::vector<Tile<Housing>> house;

        for (auto y = 0; WorldSettings::nb_tiles.y > y; y++) {
            for (auto x = 0; WorldSettings::nb_tiles.x > x ; x++) {
                sf::Vector2f worldPos{static_cast<float>(x) * WorldSettings::tile_size.x, static_cast<float>(y) * WorldSettings::tile_size.y};
                switch (h) {
                    case Housing::kHunterHouse:
                        house.emplace_back(worldPos, Housing::kHunterHouse, false);
                        break;
                    case Housing::kMinerHouse:
                        house.emplace_back(worldPos, Housing::kMinerHouse, false);
                        break;
                    case Housing::kWoodCutterHouse:
                        house.emplace_back(worldPos, Housing::kWoodCutterHouse, false);
                        break;
                    case Housing::kNone:
                        house.emplace_back(worldPos, Housing::kNone, false);

                }
            }
        }
        return house;
    }

};


#endif //CITYBUILDER_TILEMAP_GENERATOR_H
