#include "ai/npc_manager.h"
#include "worldSettings/world_settings.h"
#include <memory> // Add this include for std::make_unique

namespace api::ai {

    NpcManager& NpcManager::GetInstance() {
        static NpcManager instance;
        return instance;
    }

    void NpcManager::SetupManager(int num_npcs, const Tilemap& _map, ResourceManager& resource_manager, sf::Vector2i spawn_pos){
        resource_manager_ = &resource_manager;
        npcs_.reserve(num_npcs);
        for (int i = 0; i < num_npcs; ++i) {
            // Corrected line: Create a new NormalizeNpc object and move a unique_ptr to it into the vector
            npcs_.emplace_back(std::make_unique<NormalizeNpc>());
            npcs_.back()->Setup("_assets/kenney_medieval-rts/PNG/Default size/Unit/medievalUnit_01.png", spawn_pos, _map, *resource_manager_, Housing::kWoodCutterHouse);
        }

    }

    void NpcManager::AddNpc(int num_npcs, const Tilemap& _map, Housing home, sf::Vector2i spawn_pos) {
        for (int i = 0; i < num_npcs; ++i) {
            npcs_.emplace_back(std::make_unique<NormalizeNpc>());
            npcs_.back()->Setup("_assets/kenney_medieval-rts/PNG/Default size/Unit/medievalUnit_01.png", spawn_pos, _map, *resource_manager_, home);
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