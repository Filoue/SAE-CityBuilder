#include "ai/npc_manager.h"
#include <memory> // Add this include for std::make_unique

namespace api::ai {

    void NpcManager::SetupManager(int num_npcs, sf::Vector2i world_size, const Tilemap& _map, float _tile_size, sf::Vector2f _grid_offset){
        npcs_.reserve(num_npcs);
        for (int i = 0; i < num_npcs; ++i) {
            // Corrected line: Create a new NormalizeNpc object and move a unique_ptr to it into the vector
            npcs_.emplace_back(std::make_unique<NormalizeNpc>());
            npcs_.back()->Setup("_assets/kenney_medieval-rts/PNG/Default size/Unit/medievalUnit_01.png", world_size, set_start_position(world_size), _map, _tile_size, _grid_offset);
        }

    }
    sf::Vector2i NpcManager::set_start_position(sf::Vector2i world_size){
        std::uniform_int_distribution<int> x_dist(0, world_size.x);
        std::uniform_int_distribution<int> y_dist(0, world_size.y);
        return {0,0};
    }

    void NpcManager::AddNpc(int num_npcs, sf::Vector2i world_size, const Tilemap& _map, float _tile_size, sf::Vector2f _grid_offset) {
        for (int i = 0; i < num_npcs; ++i) {
            npcs_.emplace_back(std::make_unique<NormalizeNpc>());
            npcs_.back()->Setup("_assets/kenney_medieval-rts/PNG/Default size/Unit/medievalUnit_01.png", world_size, set_start_position(world_size), _map, _tile_size, _grid_offset);
        }
    }

    void NpcManager::Update(const float dt)const {
        for (const auto & n : npcs_)
        {
            n->Update(dt);
        }
    }

    void NpcManager::Draw(sf::RenderWindow &window)const{
        for (const auto & n : npcs_)
        {
            n->Draw(window);
        }
    }

}  // namespace api::ai