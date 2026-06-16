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
#include "../../../game/include/tilemap.h" // Include Tilemap

namespace api::ai {
    class NormalizeNpc {
    public:
        NormalizeNpc() = default;
        // Modified Setup to take start_grid_position
        void Setup(std::string_view sprite_path, sf::Vector2i world_size, sf::Vector2i start_grid_position,
                   const Tilemap& tilemap, float tile_size, sf::Vector2f grid_offset);
        void Update(float dt);
        void Draw(sf::RenderWindow& window);

        [[nodiscard]] sf::Vector2f GetPosition() const; // New method to get current world position

        // New method to set a destination and trigger pathfinding
        void SetTargetGridPosition(sf::Vector2i grid_pos);

    private:
        // Behaviour-Tree actions
        [[nodiscard]] core::ai::behaviour_tree::Status PickRandomDestination();
        [[nodiscard]] core::ai::behaviour_tree::Status MoveToDestination(); // No longer const as it modifies path_index
        [[nodiscard]] core::ai::behaviour_tree::Status Locked();

        // A* related members
        std::vector<sf::Vector2i> current_path_; // Stores the path from A*
        size_t current_path_index_ = 0;         // Current index in the path
        sf::Vector2i target_grid_position_;     // The ultimate target for A*
        const Tilemap* tilemap_ptr_ = nullptr;  // Pointer to the game's tilemap

        static constexpr float kSpeed = 200.f;

        std::unique_ptr<sf::Texture> texture_ = std::make_unique<sf::Texture>();
        std::optional<sf::Sprite> sprite_;

        motion::Motor motor_;

        std::unique_ptr<core::ai::behaviour_tree::Node> bt_root_;

        sf::Vector2i world_size_{};
        std::mt19937 rng_{std::random_device{}()};

        // Helper for coordinate conversions
        float tile_size_ = 0.f;
        sf::Vector2f grid_offset_{};

        // Helper to convert grid position to world position
        sf::Vector2f GridToWorld(sf::Vector2i grid_pos) const;
        // Helper to convert world position to grid position
        sf::Vector2i WorldToGrid(sf::Vector2f world_pos) const;
    };
}

#endif //CITYBUILDER_NORMALIZE_NPC_H
