#ifndef API_MOTION_A_STAR_H
#define API_MOTION_A_STAR_H

#include <vector>
#include <SFML/System/Vector2.hpp> // For sf::Vector2i
#include <queue> // For std::priority_queue
#include <map>   // For std::map
#include <functional> // For std::greater

// Forward declaration of Tilemap to avoid circular dependencies if Tilemap also includes AStar.
// However, AStar needs the full definition of Tilemap to access its members (terrain, gridSize_).
// So, we will include the full header here.
#include "tilemap.h" // AStar needs to know about Tilemap

namespace api::motion {

// Node structure for A* algorithm
struct AStarNode {
    sf::Vector2i position;
    float g_score; // Cost from start to this node
    float h_score; // Heuristic cost from this node to end
    float f_score; // g_score + h_score

    AStarNode(sf::Vector2i pos, float g, float h)
        : position(pos), g_score(g), h_score(h), f_score(g + h) {}

    // Comparison operator for priority queue (min-heap)
    // std::priority_queue is a max-heap by default, so we use std::greater to make it a min-heap
    bool operator>(const AStarNode& other) const {
        return f_score > other.f_score;
    }
};

class AStar {
public:
    // Finds the shortest path from start_pos to end_pos on the given tilemap.
    // Returns a vector of grid positions (sf::Vector2i) representing the path.
    // Returns an empty vector if no path is found.
    static std::vector<sf::Vector2i> FindPath(
        const sf::Vector2i &start_pos,
        const sf::Vector2i &end_pos,
        const Tilemap&tilemap_instance
    );


    template <class Predicate>
    static sf::Vector2i FindClosestTarget(sf::Vector2i start_pos,
        const Tilemap&tilemap_instance,
        Predicate criteria) {
        const sf::Vector2i& grid_size = tilemap_instance.gridSize_;

        //Sécurité initiale
        if (start_pos.x < 0 || start_pos.x >= grid_size.x ||
            start_pos.y < 0 || start_pos.y >= grid_size.y) {
            return {-1, -1}; // Non trouvé / Invalide
            }

        if (criteria(start_pos)) {
            return start_pos;
        }

        std::vector<bool> visited(grid_size.x * grid_size.y, false);

        std::queue<sf::Vector2i> open_set;

        open_set.push(start_pos);
        visited[start_pos.y * grid_size.x + start_pos.x] = true;

        struct Direction { int dx; int dy; };
        static constexpr Direction kDirection[] = { {0,-1}, {0,1}, {-1, 0}, {1,0} };

        while (!open_set.empty()) {
            sf::Vector2i current = open_set.front();
            open_set.pop();

            for (const auto& dir : kDirection) {
                sf::Vector2i neighbor{current.x + dir.dx, current.y + dir.dy};

                if (neighbor.x < 0 || neighbor.x >= grid_size.x ||
                    neighbor.y < 0 || neighbor.y >= grid_size.y) {
                    continue;
                    }

                int index = neighbor.y * grid_size.x + neighbor.x;
                if (visited[index]) {
                    continue;
                }

                if (!IsValidAndWalkable(neighbor, tilemap_instance)) {
                    if (criteria(neighbor)) {
                        return neighbor;
                    }
                    continue;
                    }

                if (criteria(neighbor)) {
                    return neighbor;
                }

                visited[index] = true;
                open_set.push(neighbor);
            }

        }

        return { -1, -1};
    }

private:
    // Helper functions (static members as they don't depend on AStar instance state)
    static int CalculateHeuristic(const sf::Vector2i &pos1, const sf::Vector2i &pos2);
    static float CalculateDistance(sf::Vector2i pos1, sf::Vector2i pos2);
    static void GetNeighbors(sf::Vector2i pos, const sf::Vector2i& grid_size, std::vector<sf::Vector2i>& neighbors);
    static bool IsValidAndWalkable(const sf::Vector2i &pos, const Tilemap&tilemap_instance);
    static std::vector<sf::Vector2i> ReconstructPath(
        const std::vector<int>&came_from,int current_index,const sf::Vector2i&grid_size
    );
};

} // namespace api::motion

#endif // API_MOTION_A_STAR_H
