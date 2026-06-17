//
// Created by sebas on 20.05.2026.
//

#ifndef CITYBUILDER_TILEMAP_GENERATOR_H
#define CITYBUILDER_TILEMAP_GENERATOR_H

#include <random>
#include <ranges>
#include <span>

#include "tile.h"

namespace tiles::generator {

    using namespace api::tiles;
    inline std::vector<Tile<TerrainTiles>> GenerateTerrain(sf::Vector2i size, sf::Vector2f tileSize){

        std::vector<Tile<TerrainTiles>> terrainMap;
        terrainMap.reserve(static_cast<size_t>(size.x) * size.y);

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
        for (int y = 0; y < size.y; y++) {
            for (int x = 0; x < size.x; x++) {
                // Calcul direct de la position monde pour éviter les décalages flottants
                sf::Vector2f worldPos{static_cast<float>(x) * tileSize.x, static_cast<float>(y) * tileSize.y};

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

    inline std::vector<Tile<RessourcesTiles>> SeedAndGrow(std::span<Tile<TerrainTiles>> terrain, RessourcesTiles _seed, TerrainTiles _terrain){

        std::vector<Tile<RessourcesTiles>> ressourceMap;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution rnd(0.f, 1.f);

        auto map = terrain
        | std::views::filter([_terrain] (auto tile){ return tile.type == _terrain;})
        | std::views::filter([&rnd, &gen] (auto tile){ return rnd(gen) <= 0.25f;})
        | std::views::transform([&_seed] (auto tile){ return Tile<RessourcesTiles>{tile.pos, _seed, true};});

        for (auto tile: map) {
            ressourceMap.emplace_back(tile);
        }
        return ressourceMap;

    }

};


#endif //CITYBUILDER_TILEMAP_GENERATOR_H
