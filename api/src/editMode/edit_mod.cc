//
// Created by petitfiloue on 19.06.2026.
//
#include "editMode/edit_mod.h"
#include "tilemap.h"
#include <cmath>

void EditMode::Setup(float tile_size, sf::Vector2f grid_offset) {
    tile_size_ = tile_size;
    grid_offset_ = grid_offset;
}

void EditMode::HandleEvent(const sf::Event& event, const sf::RenderWindow& window, const Tilemap& tilemap) {
    if (!enabled_) return;

    if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseButton->button == sf::Mouse::Button::Left) {
            // Check if position is walkable (not water)
            TerrainTiles tile = tilemap.GetTerrainTileType(hovered_grid_pos_);
            if (tile != TerrainTiles::kWaterA && tile != TerrainTiles::kWaterB) {
                // Check if already occupied (simple check)
                bool occupied = false;
                for (const auto& b : buildings_) {
                    if (b.gridPos == hovered_grid_pos_) { occupied = true; break; }
                }

                if (!occupied) {
                    buildings_.push_back({hovered_grid_pos_, current_selection_});
                }
            }
        }
    }
}

void EditMode::Update(const sf::RenderWindow& window, const Tilemap& tilemap) {
    if (!enabled_) return;

    // Get mouse position in world coordinates (accounting for camera/view)
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
    
    hovered_grid_pos_ = WorldToGrid(worldPos);
}

void EditMode::Draw(sf::RenderWindow& window) {
    // 1. Draw already placed buildings
    for (const auto& b : buildings_) {
        sf::RectangleShape shape(sf::Vector2f(tile_size_ * 0.8f, tile_size_ * 0.8f));
        shape.setOrigin(shape.getSize() / 2.f);
        shape.setPosition(GridToWorld(b.gridPos));
        shape.setFillColor(sf::Color::Blue); // Placeholder color for buildings
        window.draw(shape);
    }

    // 2. Draw "Ghost" building at cursor
    if (enabled_) {
        sf::RectangleShape ghost(sf::Vector2f(tile_size_, tile_size_));
        ghost.setOrigin(ghost.getSize() / 2.f);
        ghost.setPosition(GridToWorld(hovered_grid_pos_));
        ghost.setFillColor(sf::Color(255, 255, 255, 127)); // Semi-transparent white
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
