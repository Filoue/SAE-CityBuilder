#ifndef API_AI_NPC_MANAGER_H
#define API_AI_NPC_MANAGER_H

#include <vector>
#include "ai/normalize_npc.h"

namespace api::ai {

class NpcManager {
 public:
    NpcManager() = default;
    void SetupManager(int num_npcs, sf::Vector2f world_size);
    void Update(float dt)const;
    void Draw(sf::RenderWindow& window)const;
 private:
  std::vector<std::unique_ptr<NormalizeNpc>> npcs_;
};

}  // namespace api::ai

#endif  // API_AI_NPC_MANAGER_H
