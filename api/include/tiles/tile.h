//
// Created by sebas on 20.05.2026.
//

#ifndef CITYBUILDER_TILE_H
#define CITYBUILDER_TILE_H
#include <type_traits>
#include <SFML/System/Vector2.hpp>

namespace api::tiles {

    template<typename T>
    requires std::is_enum_v<T>
    struct Tile {
        sf::Vector2f  pos;
        T type;
        bool is_walkable;
    };

}

#endif //CITYBUILDER_TILE_H
