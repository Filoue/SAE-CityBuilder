#include "motion/a_star.h"

#include <cmath> // For std::sqrt, std::abs
#include <set>   // For std::set (to check if node is in open_set efficiently)
#include <limits> // For std::numeric_limits
#include <algorithm> // For std::reverse
#include <vector>

// // Define operator< for sf::Vector2i to allow its use as a key in std::map
namespace api::motion {

// --- Helper Functions Implementation ---

int AStar::CalculateHeuristic(const sf::Vector2i& pos1,const sf::Vector2i& pos2) {
    // Distance de Manhattan : Utilisée ici car le mouvement est restreint aux 4 directions cardinales.
    // C'est une estimation (H) du coût restant pour atteindre la cible.
    return std::abs(pos1.x - pos2.x) + std::abs(pos1.y - pos2.y);
}

bool AStar::IsValidAndWalkable(const sf::Vector2i& pos, const Tilemap& tilemap_instance) {
    // Sécurité : vérifie à nouveau les limites de la grille
    if (pos.x < 0 || pos.x >= api::tiles::WorldSettings::nb_tiles.x ||
        pos.y < 0 || pos.y >= api::tiles::WorldSettings::nb_tiles.y) {
        return false;
    }

    // Récupère le type de terrain pour déterminer si l'IA peut marcher dessus
    TerrainTiles tile_type = tilemap_instance.GetTerrainTileType(pos);


    switch (tile_type) {
        case TerrainTiles::kGrassA:
        case TerrainTiles::kGrassB:
        case TerrainTiles::kSandA:
        case TerrainTiles::kSandB:
        case TerrainTiles::kForest:
             return true; // Terrains marchables
        case TerrainTiles::kWaterA:
        case TerrainTiles::kWaterB:
            return false; // Obstacles (Eau)
        default:
            return false;
    }

}

std::vector<sf::Vector2i> AStar::ReconstructPath(
    const std::vector<int>& came_from,
    int current_index) {

    // Reconstruit le chemin en partant de la fin (Target) vers le début (Start)
    std::vector<sf::Vector2i> total_path;
    total_path.reserve(32);

    while (current_index != -1) {
        total_path.push_back(api::tiles::WorldSettings::IdxToTilePos(current_index));
        current_index = came_from[current_index];
    }
    
    // On inverse le vecteur pour avoir le chemin du début vers la fin
    std::reverse(total_path.begin(), total_path.end()); 
    return total_path;
}

// --- Main A* Algorithm Implementation ---

std::vector<sf::Vector2i> AStar::FindPath(
    const sf::Vector2i& start_pos,
    const sf::Vector2i& end_pos,
    const Tilemap& tilemap_instance) {

    // Validation initiale : si le point de départ ou d'arrivée est dans l'eau
    if (!IsValidAndWalkable(start_pos, tilemap_instance) || !IsValidAndWalkable(end_pos, tilemap_instance)) {
        return {}; 
    }
    const sf::Vector2i& grid_size = api::tiles::WorldSettings::nb_tiles;
    int total_cells = grid_size.x * grid_size.y;
    int start_index = api::tiles::WorldSettings::TilePosToIdx(start_pos);
    int end_index = api::tiles::WorldSettings::TilePosToIdx(end_pos);

    // Liste des nœuds à explorer, triée par f_score (coût total estimé) le plus bas
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> open_set;

    std::vector<float> g_scores(total_cells, std::numeric_limits<float>::max());
    std::vector<int> came_from(total_cells, -1);
    std::vector<bool> closed_set(total_cells, false);

    struct Direction { int dx; int dy; };
    static constexpr Direction kDirection[] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };

    // Initialisation avec le nœud de départ
    AStarNode start_node(start_pos, 0.0f, CalculateHeuristic(start_pos, end_pos));
    open_set.push(start_node);
    g_scores[start_index] = 0.0f;

    while (!open_set.empty()) {
        // On récupère le nœud ayant le plus petit f_score (le plus prometteur)
        AStarNode current_node = open_set.top();
        open_set.pop();

        int current_index = api::tiles::WorldSettings::TilePosToIdx(current_node.position);

        // // Condition de victoire : on a atteint la destination
        if (current_index == end_index) {
            return ReconstructPath(came_from, current_index);
        }

        if (closed_set[current_index]) continue;
        closed_set[current_index] = true;

        // Exploration des 4 voisins (Haut, Bas, Gauche, Droite)
        for (const auto& dir : kDirection) {
            sf::Vector2i neighbor_pos(current_node.position.x + dir.dx, current_node.position.y + dir.dy);

            if (neighbor_pos.x < 0 || neighbor_pos.x >= grid_size.x ||
                neighbor_pos.y < 0 || neighbor_pos.y >= grid_size.y) {
                continue;
            }

            int neighbor_index = api::tiles::WorldSettings::TilePosToIdx(neighbor_pos);

            if (closed_set[neighbor_index]) continue;

            // Si le voisin est un obstacle (eau), on l'ignore
            if (!IsValidAndWalkable(neighbor_pos, tilemap_instance)) {
                continue; 
            }

            // Calcul du score G potentiel pour ce voisin (distance parcourue + 1)
            float tentative_g_score = current_node.g_score + 1.0f;

            // Si on a trouvé un chemin plus court vers ce voisin que celui déjà connu
            if (tentative_g_score < g_scores[neighbor_index]) {
                // Enregistre ce chemin comme le meilleur pour l'instant
                came_from[neighbor_index] = current_index;
                g_scores[neighbor_index] = tentative_g_score;

                // Calcule l'estimation H et ajoute le voisin à la liste d'exploration
                float neighbor_h_score = CalculateHeuristic(neighbor_pos, end_pos);
                AStarNode neighbor_node(neighbor_pos, tentative_g_score, neighbor_h_score);
                open_set.push(neighbor_node);
            }
        }
    }

    return {}; // Aucun chemin trouvé après exploration
}

} // namespace api::motion
