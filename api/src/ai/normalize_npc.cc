#include "ai/normalize_npc.h"
#include "worldSettings/world_settings.h"

#include <cmath>
#include <print>
#include <random> // For std::uniform_int_distribution

#include "motion/a_star.h"

#include "ai/bt_node_factory.h"
#include "ai/bt_action.h"
#include "ai/bt_sequence.h"
#include "ai/bt_selector.h"


namespace api::ai {
    using core::ai::behaviour_tree::Status;

    void NormalizeNpc::Setup(std::string_view sprite_path, sf::Vector2i start_grid_position,
                               const Tilemap& tilemap, ResourceManager& resource_manager, Housing home) {
        tilemap_ptr_ = &tilemap; // Store pointer to the Tilemap
        resource_manager_ = &resource_manager;
        home_ = home;
        home_pos_ = start_grid_position;
        texture_ = std::make_unique<sf::Texture>();

        if (texture_->loadFromFile(std::string(sprite_path))) {
            sprite_ = sf::Sprite(*texture_);
            sf::FloatRect bounds = sprite_->getLocalBounds();
            sprite_->setOrigin(bounds.getCenter());
        }

        // Correctly set initial motor position to the center of the start grid tile
        sf::Vector2f start_pos = sf::Vector2f(tiles::WorldSettings::IdxToPixelPos(tiles::WorldSettings::TilePosToIdx(start_grid_position))) + sf::Vector2f(tiles::WorldSettings::tile_size.x / 2.f, tiles::WorldSettings::tile_size.y / 2.f);
        motor_.SetPosition(start_pos);
        motor_.SetDestination(start_pos); // Initial destination is also the start position
        motor_.SetSpeed(kSpeed);

        using namespace core::ai::behaviour_tree;
        using namespace node_factory;

        auto goToResource = MakeAction([this]{return GoToResource();});
        auto goHome = MakeAction([this]{return GoHome();});
        auto collectResource = MakeAction([this]{return CollectResource();});
        auto depositResource = MakeAction([this]{return DepositResource();});

        auto collectSequence = std::make_unique<SequenceNode>();
        collectSequence->AddChild(std::move(goToResource));
        collectSequence->AddChild(std::move(collectResource));

        auto depositSequence = std::make_unique<SequenceNode>();
        depositSequence->AddChild(std::move(goHome));
        depositSequence->AddChild(std::move(depositResource));

        auto rootSelector = std::make_unique<SelectorNode>();
        rootSelector->AddChild(std::move(collectSequence));
        rootSelector->AddChild(std::move(depositSequence));

        bt_root_ = std::move(rootSelector);
    }

    void NormalizeNpc::Update(float dt){
        motor_.Update(dt);

        std::print("NPC Position: ({}, {}) | dt: {}\n", motor_.GetPosition().x, motor_.GetPosition().y, dt);
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

    Status NormalizeNpc::GoToResource() {
        if (has_resource_) {
            return Status::kFailure;
        }

        if (!current_path_.empty()) {
            return Status::kSuccess;
        }

        sf::Vector2i position = tiles::WorldSettings::IdxToTilePos(tiles::WorldSettings::PixelPosToIdx(motor_.GetPosition()).value_or(0));

        RessourcesTiles target_resource;
        switch (home_) {
            case Housing::kWoodCutterHouse:
                target_resource = RessourcesTiles::kWood;
                break;
            case Housing::kMinerHouse:
                target_resource = RessourcesTiles::kRock;
                break;
            default:
                return Status::kFailure;
        }

        sf::Vector2i closest_resource = motion::AStar::FindClosestTarget(position, *tilemap_ptr_, [&](sf::Vector2i pos) {
            auto r = tilemap_ptr_->GetRessourcesTileType(pos);
            if (r) {
                return r == target_resource;
            }
            return false;
        });

        if (closest_resource.x != -1 && closest_resource.y != -1) {
            SetTargetGridPosition(closest_resource);
            target_resource_ = target_resource;
            return Status::kSuccess;
        }

        return  Status::kFailure;
    }

    Status NormalizeNpc::GoHome() {
        if (!has_resource_) {
            return Status::kFailure;
        }

        if (!current_path_.empty()) {
            return Status::kSuccess;
        }

        SetTargetGridPosition(home_pos_);
        return Status::kSuccess;
    }

    Status NormalizeNpc::CollectResource() {
        if (motor_.RemainingDistance() > 0.01f) {
            return Status::kRunning;
        }

        // Remove the resource from the map
        // This is a placeholder. A more robust solution would be to have a way to modify the tilemap.
        has_resource_ = true;
        return Status::kSuccess;
    }

    Status NormalizeNpc::DepositResource() {
        if (motor_.RemainingDistance() > 0.01f) {
            return Status::kRunning;
        }

        switch (target_resource_) {
            case RessourcesTiles::kWood:
                resource_manager_->AddWood(1);
                break;
            case RessourcesTiles::kRock:
                resource_manager_->AddStone(1);
                break;
            default:
                break;
        }

        has_resource_ = false;
        return Status::kSuccess;
    }

    void NormalizeNpc::SetTargetGridPosition(sf::Vector2i grid_pos) {
        if (!tilemap_ptr_) {
            current_path_.clear();
            current_path_index_ = 0;
            return;
        }

        target_grid_position_ = grid_pos;
        sf::Vector2i start_grid_pos = tiles::WorldSettings::IdxToTilePos(tiles::WorldSettings::PixelPosToIdx(motor_.GetPosition()).value_or(0));

        // Ensure start_grid_pos is within bounds before pathfinding
        if (start_grid_pos.x < 0 || start_grid_pos.x >= static_cast<int>(api::tiles::WorldSettings::nb_tiles.x) ||
            start_grid_pos.y < 0 || start_grid_pos.y >= static_cast<int>(api::tiles::WorldSettings::nb_tiles.y)) {
            current_path_.clear();
            current_path_index_ = 0;
            return;
        }

        current_path_ = motion::AStar::FindPath(start_grid_pos, target_grid_position_, *tilemap_ptr_);
        current_path_index_ = 0;

        if (!current_path_.empty()) {
            // Set the first waypoint as the motor's destination
            if (current_path_.size() > 1 && current_path_[0] == start_grid_pos) {
                current_path_index_ = 1;
            }else {
                current_path_index_ = 0;
            }
            motor_.SetDestination(sf::Vector2f(tiles::WorldSettings::IdxToPixelPos(tiles::WorldSettings::TilePosToIdx(current_path_[current_path_index_]))) + sf::Vector2f(tiles::WorldSettings::tile_size.x / 2.f, tiles::WorldSettings::tile_size.y / 2.f));
        } else {
            // If no path found, stay at current position
            motor_.SetDestination(motor_.GetPosition());
            // Do NOT call PickRandomDestination() here. Let the BT handle retrying.
        }
    }
}
