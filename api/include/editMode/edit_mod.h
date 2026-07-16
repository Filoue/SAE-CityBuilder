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
#include "graphics/building_renderer.h"
#include "worldSettings/world_settings.h"
#include "resource_manager.h"

class Tilemap; // Forward declaration

class EditMode {
public:
    struct PlacedBuilding {
        sf::Vector2i gridPos;
        Housing type;
    };

    void Setup(float tile_size, sf::Vector2f grid_offset, Tilemap& tilemap, ResourceManager& resource_manager);
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void Update(const sf::RenderWindow& window);void Building();
    void Draw(sf::RenderWindow& window);

    void SetSelectedBuilding(Housing type) { current_selection_ = type; }
    bool IsEnabled() const { return enabled_; }
    void Toggle() { enabled_ = !enabled_; }

    std::vector<PlacedBuilding> buildings_;

private:
    bool Placable(TerrainTiles tile);

    int selection = 0;
    std::vector<sf::RectangleShape> buildinglist_;
    sf::Color color_;

    BuildingRenderer building_renderer_;

    std::unique_ptr<sf::Texture> hunterHouse_;
    std::unique_ptr<sf::Texture> minerHouse_;
    std::unique_ptr<sf::Texture> woodCutterHouse_;

    ResourceManager* resource_manager_;


    bool enabled_ = false;
    float tile_size_ = 32.f;
    sf::Vector2f grid_offset_ = {0.f, 0.f};
    Housing current_selection_ = Housing::kHunterHouse;
    sf::Vector2i hovered_grid_pos_;
    Tilemap* tilemap_;
    Housing housing_;
};
#endif //CITYBUILDER_EDIT_MOD_H
