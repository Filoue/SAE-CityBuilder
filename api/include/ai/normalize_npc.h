#ifndef CITYBUILDER_NORMALIZE_NPC_H
#define CITYBUILDER_NORMALIZE_NPC_H

#include <memory>
#include <optional>
#include <random>
#include <string_view>
#include <vector> // For std::vector<sf::Vector2i>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

#include "ai/bt_node.h"
#include "motion/motor.h"
#include "motion/a_star.h" // Include AStar
#include "tilemap.h" // Include Tilemap
#include "resource_manager.h"

namespace api::ai {
    class NormalizeNpc {
    public:
        NormalizeNpc() = default;
        // Modified Setup to take start_grid_position
        void Setup(std::string_view sprite_path, sf::Vector2i start_grid_position,
                   const Tilemap& tilemap, ResourceManager& resource_manager, Housing home);
        void Update(float dt);
        void Draw(sf::RenderWindow& window);

        [[nodiscard]] sf::Vector2f GetPosition() const; // New method to get current world position

        // New method to set a destination and trigger pathfinding
        void SetTargetGridPosition(sf::Vector2i grid_pos);

    private:
        // Behaviour-Tree actions
        [[nodiscard]] core::ai::behaviour_tree::Status GoToResource();
        [[nodiscard]] core::ai::behaviour_tree::Status GoHome();
        [[nodiscard]] core::ai::behaviour_tree::Status CollectResource();
        [[nodiscard]] core::ai::behaviour_tree::Status DepositResource();

        // A* related members
        std::vector<sf::Vector2i> current_path_; // Stores the path from A*
        size_t current_path_index_ = 0;         // Current index in the path
        sf::Vector2i target_grid_position_;     // The ultimate target for A*
        const Tilemap* tilemap_ptr_ = nullptr;  // Pointer to the game's tilemap
        ResourceManager* resource_manager_ = nullptr;
        Housing home_;
        sf::Vector2i home_pos_;
        RessourcesTiles target_resource_;
        bool has_resource_ = false;

        static constexpr float kSpeed = 200.f;

        std::unique_ptr<sf::Texture> texture_;
        std::optional<sf::Sprite> sprite_;

        motion::Motor motor_;

        std::unique_ptr<core::ai::behaviour_tree::Node> bt_root_;

        sf::Vector2i closeest_rock_;

        std::mt19937 rng_{std::random_device{}()};
    };
}

#endif //CITYBUILDER_NORMALIZE_NPC_H
