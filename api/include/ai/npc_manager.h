#ifndef API_AI_NPC_MANAGER_H
#define API_AI_NPC_MANAGER_H

#include <vector>
#include "ai/normalize_npc.h"
#include "resource_manager.h"

namespace api::ai {

class NpcManager {
 public:
    static NpcManager& GetInstance();
    void SetupManager(int num_npcs, const Tilemap& _map, ResourceManager& resource_manager, sf::Vector2i spawn_pos);
    void Update(float dt)const;
    void AddNpc(int num_npcs, const Tilemap&_map, Housing home, sf::Vector2i spawn_pos);
    void Draw(sf::RenderWindow& window)const;
 private:
    NpcManager() = default;

  std::vector<std::unique_ptr<NormalizeNpc>> npcs_;
    sf::Vector2f startPosition_;
    std::mt19937 rng_;
    ResourceManager* resource_manager_ = nullptr;
};

}  // namespace api::ai

#endif  // API_AI_NPC_MANAGER_H
