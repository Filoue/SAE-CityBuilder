//
// Created by petitfiloue on 19.06.2026.
//

#ifndef CITYBUILDER_EDIT_MOD_H
#define CITYBUILDER_EDIT_MOD_H

#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include "game_types.h"
#include "tilemap.h"

class Tilemap; // Forward declaration

class EditMode {
public:
    struct PlacedBuilding {
        sf::Vector2i gridPos;
        Housing type;
    };

    void Setup(float tile_size, sf::Vector2f grid_offset, Tilemap* tilemap);
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void Update(const sf::RenderWindow& window);
    void Draw(sf::RenderWindow& window);

    void SetSelectedBuilding(Housing type) { current_selection_ = type; }
    bool IsEnabled() const { return enabled_; }
    void Toggle() { enabled_ = !enabled_; }

private:
    sf::Vector2i WorldToGrid(sf::Vector2f world_pos) const;
    sf::Vector2f GridToWorld(sf::Vector2i grid_pos) const;
    bool Placable(TerrainTiles tile);




    bool enabled_ = false;
    float tile_size_ = 32.f;
    sf::Vector2f grid_offset_ = {0.f, 0.f};
    Housing current_selection_ = Housing::kHunterHouse;
    sf::Vector2i hovered_grid_pos_;
    std::vector<PlacedBuilding> buildings_;
    Tilemap* tilemap_;
};
#endif //CITYBUILDER_EDIT_MOD_H
