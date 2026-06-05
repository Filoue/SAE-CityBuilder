#include "ai/npc_manager.h"
#include <memory> // Add this include for std::make_unique

namespace api::ai {

    void NpcManager::SetupManager(int num_npcs, sf::Vector2f world_size){
        npcs_.reserve(num_npcs);
        for (int i = 0; i < num_npcs; ++i) {
            // Corrected line: Create a new NormalizeNpc object and move a unique_ptr to it into the vector
            npcs_.emplace_back(std::make_unique<NormalizeNpc>());
            npcs_.back()->Setup("_assets/kenney_medieval-rts/PNG/Default size/Unit/medievalUnit_01.png", world_size, set_start_position(world_size));
        }

    }sf::Vector2f NpcManager::set_start_position(sf::Vector2f world_size){
        std::uniform_real_distribution<float> x_dist(0.f, world_size.x);
        std::uniform_real_distribution<float> y_dist(0.f, world_size.y);
        return {x_dist(rng_), y_dist(rng_)};
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