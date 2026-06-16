#include "motion/a_star.h"

#include <cmath> // For std::sqrt, std::abs
#include <set>   // For std::set (to check if node is in open_set efficiently)
#include <limits> // For std::numeric_limits
#include <algorithm> // For std::reverse

// Define operator< for sf::Vector2i to allow its use as a key in std::map
namespace sf {
    bool operator<(const Vector2i& lhs, const Vector2i& rhs) {
        if (lhs.x != rhs.x) {
            return lhs.x < rhs.x;
        }
        return lhs.y < rhs.y;
    }
} // namespace sf

namespace api::motion {

// --- Helper Functions Implementation ---

float AStar::CalculateHeuristic(sf::Vector2i pos1, sf::Vector2i pos2) {
    // Euclidean distance heuristic
    float dx = static_cast<float>(pos1.x - pos2.x);
    float dy = static_cast<float>(pos1.y - pos2.y);
    return std::sqrt(dx * dx + dy * dy);
}

float AStar::CalculateDistance(sf::Vector2i pos1, sf::Vector2i pos2) {
    // Cost for moving from one node to a neighbor
    int dx = std::abs(pos1.x - pos2.x);
    int dy = std::abs(pos1.y - pos2.y);

    if (dx == 1 || dy == 1) { // Orthogonal move
        return 1.0f;
    }
    return 0.0f; // Same position
}

std::vector<sf::Vector2i> AStar::GetNeighbors(sf::Vector2i pos, const sf::Vector2i& grid_size) {
    std::vector<sf::Vector2i> neighbors;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) {
                continue; // Skip the node itself
            }

            sf::Vector2i neighbor_pos(pos.x + dx, pos.y + dy);

            // Check if neighbor is within grid bounds
            // Note: grid_size.x and grid_size.y are floats, need to cast to int for comparison
            if (neighbor_pos.x >= 0 && neighbor_pos.x < static_cast<int>(grid_size.x) &&
                neighbor_pos.y >= 0 && neighbor_pos.y < static_cast<int>(grid_size.y)) {
                neighbors.push_back(neighbor_pos);
            }
        }
    }
    return neighbors;
}

bool AStar::IsValidAndWalkable(sf::Vector2i pos, const Tilemap& tilemap_instance) {
    // Check if position is within grid bounds (redundant if GetNeighbors already checks, but safe)
    if (pos.x < 0 || pos.x >= static_cast<int>(tilemap_instance.gridSize_.x) ||
        pos.y < 0 || pos.y >= static_cast<int>(tilemap_instance.gridSize_.y)) {
        return false;
    }

    // Use the public getter to retrieve the tile type
    TerrainTiles tile_type = tilemap_instance.GetTerrainTileType(pos);

    // Define which TerrainTiles are walkable
    switch (tile_type) {
        case TerrainTiles::kGrassA:
        case TerrainTiles::kGrassB:
        case TerrainTiles::kSandA:
        case TerrainTiles::kSandB:
        case TerrainTiles::kForest:
            return true;// These tiles are walkable
        case TerrainTiles::kWaterA:
        case TerrainTiles::kWaterB:
            return false; // These tiles are obstacles
        default:
            return false; // Any other type is an obstacle by default
    }
}

std::vector<sf::Vector2i> AStar::ReconstructPath(
    const std::map<sf::Vector2i, sf::Vector2i>& came_from,
    sf::Vector2i current_pos) {

    std::vector<sf::Vector2i> total_path;

    total_path.push_back(current_pos);

    while (came_from.count(current_pos)) {
        current_pos = came_from.at(current_pos);
        total_path.push_back(current_pos);
    }
    std::reverse(total_path.begin(), total_path.end()); // Path is built backwards
    return total_path;
}

// --- Main A* Algorithm Implementation ---

std::vector<sf::Vector2i> AStar::FindPath(
    sf::Vector2i start_pos,
    sf::Vector2i end_pos,
    const Tilemap& tilemap_instance) {

    // Check if start or end positions are invalid or unwalkable
    if (!IsValidAndWalkable(start_pos, tilemap_instance) || !IsValidAndWalkable(end_pos, tilemap_instance)) {
        return {}; // Return empty path if start/end is invalid or unwalkable
    }

    // Priority queue for nodes to explore, ordered by f_score
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> open_set;

    // Maps to store g_score and parent for each position
    std::map<sf::Vector2i, float> g_scores_map;
    std::map<sf::Vector2i, sf::Vector2i> came_from_map;

    // Initialize start node
    AStarNode start_node(start_pos, 0.0f, CalculateHeuristic(start_pos, end_pos));
    open_set.push(start_node);
    g_scores_map[start_pos] = 0.0f;

    while (!open_set.empty()) {
        AStarNode current_node = open_set.top();
        open_set.pop();

        // If we reached the end, reconstruct and return the path
        if (current_node.position == end_pos) {
            return ReconstructPath(came_from_map, current_node.position);
        }

        // Explore neighbors
        for (const auto& neighbor_pos : GetNeighbors(current_node.position, tilemap_instance.gridSize_)) {
            // Check walkability using the dedicated function
            if (!IsValidAndWalkable(neighbor_pos, tilemap_instance)) {
                continue; // Skip obstacles or out-of-bounds
            }

            float tentative_g_score = current_node.g_score + CalculateDistance(current_node.position, neighbor_pos);

            // Check if this path to neighbor is better than any previous one
            // Use g_scores_map.count() to check if neighbor_pos is already in the map
            if (!g_scores_map.count(neighbor_pos) || tentative_g_score < g_scores_map[neighbor_pos]) {
                came_from_map[neighbor_pos] = current_node.position;
                g_scores_map[neighbor_pos] = tentative_g_score;

                float neighbor_h_score = CalculateHeuristic(neighbor_pos, end_pos);
                AStarNode neighbor_node(neighbor_pos, tentative_g_score, neighbor_h_score);

                // Add to open_set. If already present with a worse score, it will be re-added,
                // but the old one will be ignored when popped later due to g_scores_map check.
                open_set.push(neighbor_node);
            }
        }
    }

    return {}; // No path found
}

} // namespace api::motion
