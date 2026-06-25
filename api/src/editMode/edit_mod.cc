//
// Created by petitfiloue on 19.06.2026.
//
#include "editMode/edit_mod.h"
#include "tilemap.h"
#include <cmath>

void EditMode::Setup(float tile_size, sf::Vector2f grid_offset, Tilemap* tilemap) {
    tile_size_ = tile_size;
    grid_offset_ = grid_offset;
    tilemap_ = tilemap;
}

void EditMode::HandleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    //if (!enabled_) return;

    if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseButton->button == sf::Mouse::Button::Left) {
            // Check if position is walkable (not water)
            TerrainTiles tile = tilemap_->GetTerrainTileType(hovered_grid_pos_);
            if (Placable(tile)) {
                // Check if already occupied (simple check)
                bool occupied = false;
                // TODO make a unordered_set insert(pos) erase(pos) : if occupied_cells_.contains(hovered_grid_pos_
                for (const auto& b : buildings_) {
                    if (b.gridPos == hovered_grid_pos_) { occupied = true; break; }
                }

                if (!occupied) {
                    buildings_.push_back({hovered_grid_pos_, current_selection_});
                }
            }
            Building();
        }
    }
    if (const auto* keyPad = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPad->code == sf::Keyboard::Key::Numpad1) selection = 0;
        if (keyPad->code == sf::Keyboard::Key::Numpad2) selection = 1;
        if (keyPad->code == sf::Keyboard::Key::Numpad3) selection = 2;
    }
    switch (selection) {
        case 0:
            current_selection_ = Housing::kHunterHouse;
            color_ = sf::Color(0,0,255);
            break;
        case 1:
            current_selection_ = Housing::kMinerHouse;
            color_ = sf::Color(100,100,100);
            break;
        case 2:
            current_selection_ = Housing::kWoodCutterHouse;
            color_ = sf::Color(179, 79, 50);
            break;
    }
}

void EditMode::Update(const sf::RenderWindow& window) {
    if (!enabled_) return;

    // Get mouse position in world coordinates (accounting for camera/view)
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
    
    hovered_grid_pos_ = WorldToGrid(worldPos);
}

void EditMode::Building() {
    shape.setFillColor(color_);

    for (const auto& b : buildings_) {
        shape.setSize(sf::Vector2f(tile_size_ * 0.8f, tile_size_ * 0.8f));
        shape.setOrigin(shape.getSize() / 2.f);
        shape.setPosition(GridToWorld(b.gridPos));
    }

    buildinglist_.emplace_back(shape);
}

void EditMode::Draw(sf::RenderWindow& window) {
    // 1. Draw already placed buildings
    // TODO VertexArray$
    for (auto element : buildinglist_)
    {
        window.draw(element);
    }

    TerrainTiles tile = tilemap_->GetTerrainTileType(hovered_grid_pos_);
    // 2. Draw "Ghost" building at cursor
     if (enabled_) {
        sf::RectangleShape ghost(sf::Vector2f(tile_size_, tile_size_));
         ghost.setOrigin(ghost.getSize() / 2.f);
         ghost.setPosition(GridToWorld(hovered_grid_pos_));
         if (Placable(tile)) {
             ghost.setFillColor(sf::Color(color_.r, color_.g, color_.b, 100)); // Semi-transparent white
         }else if (!Placable(tile)) {
             ghost.setFillColor(sf::Color(255, 0, 0, 127));
         }

        window.draw(ghost);
    }
}

sf::Vector2i EditMode::WorldToGrid(sf::Vector2f world_pos) const {
    float adjusted_x = world_pos.x - grid_offset_.x;
    float adjusted_y = world_pos.y - grid_offset_.y;
    return sf::Vector2i(static_cast<int>(std::floor(adjusted_x / tile_size_)),
                        static_cast<int>(std::floor(adjusted_y / tile_size_)));
}

sf::Vector2f EditMode::GridToWorld(sf::Vector2i grid_pos) const {
    return sf::Vector2f(static_cast<float>(grid_pos.x) * tile_size_ + tile_size_ / 2.f,
                        static_cast<float>(grid_pos.y) * tile_size_ + tile_size_ / 2.f) + grid_offset_;
}

bool EditMode::Placable(TerrainTiles tile) {

    switch (tile) {
        case TerrainTiles::kForest:
        case TerrainTiles::kGrassA:
        case TerrainTiles::kGrassB:
        case TerrainTiles::kSandA:
        case TerrainTiles::kSandB:
            return true;
        case TerrainTiles::kWaterA:
        case TerrainTiles::kWaterB:
            return false;
        default:
            return false;
    }
}
