//
// Created by petit on 04.06.2026.
//

#include "ai/normalize_npc.h"

#include <print>
#include <random> // For std::uniform_int_distribution

#include "motion/a_star.h"

#include "ai/bt_node_factory.h"
#include "ai/bt_action.h"
#include "ai/bt_sequence.h"


namespace api::ai {
    using core::ai::behaviour_tree::Status;

    void NormalizeNpc::Setup(std::string_view sprite_path, sf::Vector2i world_size, sf::Vector2i start_grid_position,
                               const Tilemap& tilemap, float tile_size, sf::Vector2f grid_offset) {
        world_size_ = world_size;
        tilemap_ptr_ = &tilemap; // Store pointer to the Tilemap
        tile_size_ = tile_size;
        grid_offset_ = grid_offset;

        if (texture_->loadFromFile(std::string(sprite_path))) {
            sprite_ = sf::Sprite(*texture_);
        }

        // Correctly set initial motor position to the center of the start grid tile
        motor_.SetPosition(GridToWorld(start_grid_position));
        motor_.SetDestination(GridToWorld(start_grid_position)); // Initial destination is also the start position
        motor_.SetSpeed(kSpeed);

        using namespace core::ai::behaviour_tree;
        using namespace node_factory;

        auto move = MakeAction([this]{return MoveToDestination();});
        auto pick = MakeAction([this]{return PickRandomDestination();});

        auto wanderSequence = std::make_unique<SequenceNode>();
        wanderSequence->AddChild(std::move(pick));
        wanderSequence->AddChild(std::move(move));

        bt_root_ = std::move(wanderSequence);
    }

    void NormalizeNpc::Update(float dt){
        motor_.Update(dt);
        if (sprite_.has_value()) {
            sprite_->setPosition(motor_.GetPosition());
        }
        if (bt_root_) bt_root_->Tick();
    }

    void NormalizeNpc::Draw(sf::RenderWindow &window){
        if (sprite_.has_value()) {
            window.draw(*sprite_);
        }
    }

    sf::Vector2f NormalizeNpc::GetPosition() const {
        return motor_.GetPosition();
    }

    sf::Vector2f NormalizeNpc::GridToWorld(sf::Vector2i grid_pos) const {
        return sf::Vector2f(static_cast<float>(grid_pos.x) * tile_size_ + tile_size_ / 2.f,
                            static_cast<float>(grid_pos.y) * tile_size_ + tile_size_ / 2.f) + grid_offset_;
    }

    sf::Vector2i NormalizeNpc::WorldToGrid(sf::Vector2f world_pos) const {
        // Ensure world_pos is relative to grid_offset before dividing by tile_size_
        // Add a small epsilon to avoid floating point issues near tile boundaries
        float adjusted_x = world_pos.x - grid_offset_.x;
        float adjusted_y = world_pos.y - grid_offset_.y;

        // Use std::floor to correctly handle negative coordinates if they were possible,
        // but for grid indices, static_cast<int> is usually fine for positive values.
        return sf::Vector2i(static_cast<int>(adjusted_x / tile_size_),
                            static_cast<int>(adjusted_y / tile_size_));
    }

    void NormalizeNpc::SetTargetGridPosition(sf::Vector2i grid_pos) {
        if (!tilemap_ptr_) {
            std::println("Error: Tilemap pointer not set for NormalizeNpc.");
            current_path_.clear();
            current_path_index_ = 0;
            return;
        }

        target_grid_position_ = grid_pos;
        sf::Vector2i start_grid_pos = WorldToGrid(motor_.GetPosition());

        // Ensure start_grid_pos is within bounds before pathfinding
        if (start_grid_pos.x < 0 || start_grid_pos.x >= static_cast<int>(tilemap_ptr_->gridSize_.x) ||
            start_grid_pos.y < 0 || start_grid_pos.y >= static_cast<int>(tilemap_ptr_->gridSize_.y)) {
            std::println("Error: NPC's current grid position ({}, {}) is out of bounds for pathfinding.", start_grid_pos.x, start_grid_pos.y);
            current_path_.clear();
            current_path_index_ = 0;
            return;
        }

        current_path_ = motion::AStar::FindPath(start_grid_pos, target_grid_position_, *tilemap_ptr_);
        current_path_index_ = 0;

        if (!current_path_.empty()) {
            // Set the first waypoint as the motor's destination
            motor_.SetDestination(GridToWorld(current_path_[current_path_index_]));
        } else {
            // If no path found, stay at current position
            motor_.SetDestination(motor_.GetPosition());
            std::println("No path found to target grid position: {}, {}", grid_pos.x, grid_pos.y);
            // Do NOT call PickRandomDestination() here. Let the BT handle retrying.
        }
    }

    Status NormalizeNpc::PickRandomDestination(){
        if (!tilemap_ptr_) {
            std::println("Error: Tilemap pointer not set for NormalizeNpc.");
            return Status::kFailure;
        }

        // Get grid dimensions from the tilemap
        int grid_width = static_cast<int>(tilemap_ptr_->gridSize_.x);
        int grid_height = static_cast<int>(tilemap_ptr_->gridSize_.y);

        if (grid_width <= 0 || grid_height <= 0) {
            std::println("Error: Invalid grid dimensions from Tilemap.");
            return Status::kFailure;
        }

        std::uniform_int_distribution<int> x_dist(0, grid_width - 1);
        std::uniform_int_distribution<int> y_dist(0, grid_height - 1);

        sf::Vector2i random_grid_pos;
        bool found_walkable = false;
        // Try to find a walkable random position
        for (int i = 0; i < 1000; ++i) { // Limit attempts to avoid infinite loop
            random_grid_pos = {x_dist(rng_), y_dist(rng_)};
            // Use Tilemap's public method to check walkability
            TerrainTiles tile_type = tilemap_ptr_->GetTerrainTileType(random_grid_pos);
            if (tile_type != TerrainTiles::kWaterA &&
                tile_type != TerrainTiles::kWaterB) { // kForest is now considered walkable
                found_walkable = true;
                break;
            }
        }

        if (found_walkable) {
            SetTargetGridPosition(random_grid_pos);
            if (current_path_.empty()) { // If SetTargetGridPosition failed to find a path
                std::println("PickRandomDestination: Found walkable tile but A* failed to find path.");
                return Status::kFailure;
            }
            return Status::kSuccess;
        } else {
            std::println("Could not find a walkable random destination after 1000 attempts.");
            return Status::kFailure;
        }
    }

    Status NormalizeNpc::MoveToDestination() {
        if (current_path_.empty()) {
            return Status::kFailure; // No path to follow
        }

        // Check if motor has reached the current waypoint
        if (motor_.RemainingDistance() <= 0.01f) { // Reduced epsilon for more precise waypoint arrival
            current_path_index_++;

            if (current_path_index_ < current_path_.size()) {
                // Move to the next waypoint
                motor_.SetDestination(GridToWorld(current_path_[current_path_index_]));
                return Status::kRunning;
            } else {
                // Reached the end of the path
                current_path_.clear();
                current_path_index_ = 0;
                return Status::kSuccess;
            }
        }
        return Status::kRunning; // Still moving towards current waypoint
    }

    Status NormalizeNpc::Locked() {
        std::println("Locked");
        return Status::kSuccess;
    }

}
