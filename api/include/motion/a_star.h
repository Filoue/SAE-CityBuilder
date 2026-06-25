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
        sf::Vector2i start_pos,
        sf::Vector2i end_pos,
        const Tilemap& tilemap_instance
    );

private:
    // Helper functions (static members as they don't depend on AStar instance state)
    static int CalculateHeuristic(sf::Vector2i pos1, sf::Vector2i pos2);
    static float CalculateDistance(sf::Vector2i pos1, sf::Vector2i pos2);
    static std::vector<sf::Vector2i> GetNeighbors(sf::Vector2i pos, const sf::Vector2i& grid_size);
    static bool IsValidAndWalkable(sf::Vector2i pos, const Tilemap& tilemap_instance);
    static bool IsRessourceWalkable(sf::Vector2i pos, const Tilemap&tilemap_instance);
    static std::vector<sf::Vector2i> ReconstructPath(
        const std::map<sf::Vector2i, sf::Vector2i>& came_from,
        sf::Vector2i current_pos
    );
};

} // namespace api::motion

#endif // API_MOTION_A_STAR_H
