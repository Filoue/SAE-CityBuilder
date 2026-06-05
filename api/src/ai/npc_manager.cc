#include "ai/npc_manager.h"

namespace api::ai {

    void NpcManager::SetupManager(int num_npcs, sf::Vector2f world_size){
        npcs_.reserve(num_npcs);
        for (int i = 0; i < num_npcs; ++i) {
            npcs_.emplace_back();
            npcs_.back()->Setup("_assets/kenney_medieval-rts/PNG/Default size/Unit/medievalUnit_01.png", world_size, {100,100});
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
