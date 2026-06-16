#ifndef API_AI_NPC_MANAGER_H
#define API_AI_NPC_MANAGER_H

#include <vector>
#include "ai/normalize_npc.h"

namespace api::ai {

class NpcManager {
 public:
    NpcManager() = default;
    void SetupManager(int num_npcs, sf::Vector2i world_size,const Tilemap& _map, float _tile_size, sf::Vector2f _grid_offset);
    void Update(float dt)const;
    void Draw(sf::RenderWindow& window)const;
 private:
    sf::Vector2i set_start_position(sf::Vector2i world_size);

  std::vector<std::unique_ptr<NormalizeNpc>> npcs_;
    sf::Vector2f startPosition_;
    std::mt19937 rng_;
};

}  // namespace api::ai

#endif  // API_AI_NPC_MANAGER_H
