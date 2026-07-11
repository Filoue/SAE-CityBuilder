//
// Created by sebas on 13.05.2026.
//

#ifndef CITYBUILDER_GAME_TYPES_H
#define CITYBUILDER_GAME_TYPES_H

enum class Biome {
    kPlain = 1,
    kForest = 2,
    kDesert = 3,
    kOcean = 0
};

enum class TerrainTiles {
    kGrassA,
    kGrassB,
    kWaterA,
    kWaterB,
    kSandA,
    kSandB,
    kForest,
};

enum class RessourcesTiles {
    kWood,
    kRock,
    kFood,
    kNone
};

enum class Housing {
    kHunterHouse = 0,
    kWoodCutterHouse,
    kMinerHouse,
    kNone,
};

#endif //CITYBUILDER_GAME_TYPES_H
