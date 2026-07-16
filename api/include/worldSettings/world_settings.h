//
// Created by sebas on 25.06.2026.
//

#ifndef CITYBUILDER_GRID_SETTINGS_H
#define CITYBUILDER_GRID_SETTINGS_H
#include <SFML/System/Vector2.hpp>

namespace api::tiles {

    struct WorldSettings {
        static constexpr sf::Vector2i nb_tiles = {1920 / 32, 1080 / 32};
        static constexpr sf::Vector2i tile_size = {32, 32};

        // ReSharper disable once CppParameterMayBeConst
        static constexpr size_t TilePosToIdx(sf::Vector2i tile_pos){
            return tile_pos.y * nb_tiles.x + tile_pos.x;
        }
        static constexpr size_t TilePosToIdx(sf::Vector2f tile_pos){
            const auto x = static_cast<size_t>(tile_pos.x);
            const auto y = static_cast<size_t>(tile_pos.y);
            return y * nb_tiles.x + x;
        }

        static constexpr std::optional<size_t> PixelPosToIdx(const sf::Vector2i pixel_pos){

            const size_t x = (pixel_pos.x - pixel_pos.x % tile_size.x) / tile_size.x;
            const size_t y = (pixel_pos.y - pixel_pos.y % tile_size.y) / tile_size.y;

            if (x < nb_tiles.x && y < nb_tiles.y) {
                return y * nb_tiles.x + x;
            }

            return std::nullopt;
        }

        static constexpr std::optional<size_t> PixelPosToIdx(sf::Vector2f pixel_pos){
            return PixelPosToIdx(sf::Vector2i{static_cast<int>(pixel_pos.x), static_cast<int>(pixel_pos.y)});
        }

        // ReSharper disable once CppParameterMayBeConst
        static constexpr sf::Vector2i IdxToTilePos(size_t idx){
            return {static_cast<int>(idx % nb_tiles.x), static_cast<int>(idx / nb_tiles.x)};
        }

        static constexpr sf::Vector2i IdxToPixelPos(size_t idx){
            return {static_cast<int>(idx % nb_tiles.x) * tile_size.x, static_cast<int>(idx / nb_tiles.x) * tile_size.y};
        }

    };


}

#endif //CITYBUILDER_GRID_SETTINGS_H
