#include "motion/a_star.h"

#include <cmath> // For std::sqrt, std::abs
#include <set>   // For std::set (to check if node is in open_set efficiently)
#include <limits> // For std::numeric_limits
#include <algorithm> // For std::reverse

// Define operator< for sf::Vector2i to allow its use as a key in std::map
namespace sf {
    bool operator<(const sf::Vector2i& lhs, const sf::Vector2i& rhs) {
        if (lhs.x != rhs.x) {
            return lhs.x < rhs.x;
        }
        return lhs.y < rhs.y;
    }
} // namespace sf

namespace api::motion {

// --- Helper Functions Implementation ---

int AStar::CalculateHeuristic(sf::Vector2i pos1, sf::Vector2i pos2) {
    // Distance de Manhattan : Utilisée ici car le mouvement est restreint aux 4 directions cardinales.
    // C'est une estimation (H) du coût restant pour atteindre la cible.
    return std::abs(pos1.x - pos2.x) + std::abs(pos1.y - pos2.y);
}

float AStar::CalculateDistance(sf::Vector2i pos1, sf::Vector2i pos2) {
    // Coût réel (G) pour se déplacer d'une tuile à sa voisine directe.
    int dx = std::abs(pos1.x - pos2.x);
    int dy = std::abs(pos1.y - pos2.y);

    // Si les tuiles sont adjacentes, le coût est de 1.
    if (dx + dy == 1) return 1.0f;
    return 0.0f; // Même position
}

    //TODO vector to array constexpr

std::vector<sf::Vector2i> AStar::GetNeighbors(sf::Vector2i pos, const sf::Vector2i& grid_size) {
    std::vector<sf::Vector2i> neighbors;
    // Parcourt les tuiles adjacentes (incluant les diagonales dans la boucle)
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            // On filtre pour ne garder que les mouvements orthogonaux (Haut, Bas, Gauche, Droite)
            // La somme des valeurs absolues de dx et dy doit être 1.
            if (std::abs(dx) + std::abs(dy) != 1) {
                continue; 
            }

            sf::Vector2i neighbor_pos(pos.x + dx, pos.y + dy);

            // Vérification des limites de la grille
            if (neighbor_pos.x >= 0 && neighbor_pos.x < grid_size.x &&
                neighbor_pos.y >= 0 && neighbor_pos.y < grid_size.y) {
                neighbors.push_back(neighbor_pos);
            }
        }
    }
    return neighbors;
}

bool AStar::IsValidAndWalkable(sf::Vector2i pos, const Tilemap& tilemap_instance) {
    // Sécurité : vérifie à nouveau les limites de la grille
    if (pos.x < 0 || pos.x >= tilemap_instance.gridSize_.x ||
        pos.y < 0 || pos.y >= tilemap_instance.gridSize_.y) {
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

bool AStar::IsRessourceWalkable(sf::Vector2i pos, const Tilemap& tilemap_instance) {
    if (pos.x < 0 || pos.x >= tilemap_instance.gridSize_.x ||
    pos.y < 0 || pos.y >= tilemap_instance.gridSize_.y) {
        return false;
    }
    RessourcesTiles ressource_type = tilemap_instance.GetRessourcesTileType(pos);

    // TODO need to look why the png walk on trees and rocks

    switch (ressource_type) {
        case RessourcesTiles::kNone:
        case RessourcesTiles::kFood:
            return true;
        case RessourcesTiles::kRock:
        case RessourcesTiles::kWood:
            return false;
        default:
            return false;
    }
}

std::vector<sf::Vector2i> AStar::ReconstructPath(
    const std::map<sf::Vector2i, sf::Vector2i>& came_from,
    sf::Vector2i current_pos) {

    // Reconstruit le chemin en partant de la fin (Target) vers le début (Start)
    std::vector<sf::Vector2i> total_path;
    total_path.push_back(current_pos);

    // Remonte la chaîne des parents stockée dans came_from_map
    while (came_from.count(current_pos)) {
        current_pos = came_from.at(current_pos);
        total_path.push_back(current_pos);
    }
    
    // On inverse le vecteur pour avoir le chemin du début vers la fin
    std::reverse(total_path.begin(), total_path.end()); 
    return total_path;
}

// --- Main A* Algorithm Implementation ---

std::vector<sf::Vector2i> AStar::FindPath(
    sf::Vector2i start_pos,
    sf::Vector2i end_pos,
    const Tilemap& tilemap_instance) {

    // Validation initiale : si le point de départ ou d'arrivée est dans l'eau
    if (!IsValidAndWalkable(start_pos, tilemap_instance) || !IsValidAndWalkable(end_pos, tilemap_instance)) {
        return {}; 
    }
    if (!IsRessourceWalkable(start_pos, tilemap_instance) || !IsRessourceWalkable(end_pos, tilemap_instance)) {
        return {};
    }

    // Liste des nœuds à explorer, triée par f_score (coût total estimé) le plus bas
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> open_set;

    // TODO change it with vector
    // g_scores_map : stocke le coût le plus bas trouvé pour aller du départ à cette tuile
    std::map<sf::Vector2i, float> g_scores_map;
    // came_from_map : permet de savoir de quelle tuile on vient pour reconstruire le chemin
    std::map<sf::Vector2i, sf::Vector2i> came_from_map;

    // Initialisation avec le nœud de départ
    AStarNode start_node(start_pos, 0.0f, CalculateHeuristic(start_pos, end_pos));
    open_set.push(start_node);
    g_scores_map[start_pos] = 0.0f;

    while (!open_set.empty()) {
        // On récupère le nœud ayant le plus petit f_score (le plus prometteur)
        AStarNode current_node = open_set.top();
        open_set.pop();

        // Condition de victoire : on a atteint la destination
        if (current_node.position == end_pos) {
            return ReconstructPath(came_from_map, current_node.position);
        }

        // Exploration des 4 voisins (Haut, Bas, Gauche, Droite)
        for (const auto& neighbor_pos : GetNeighbors(current_node.position, tilemap_instance.gridSize_)) {
            // Si le voisin est un obstacle (eau), on l'ignore
            if (!IsValidAndWalkable(neighbor_pos, tilemap_instance)) {
                continue; 
            }
            if (!IsRessourceWalkable(neighbor_pos, tilemap_instance)) {
                continue;
            }

            // Calcul du score G potentiel pour ce voisin (distance parcourue + 1)
            float tentative_g_score = current_node.g_score + CalculateDistance(current_node.position, neighbor_pos);

            // Si on a trouvé un chemin plus court vers ce voisin que celui déjà connu
            if (!g_scores_map.count(neighbor_pos) || tentative_g_score < g_scores_map[neighbor_pos]) {
                // Enregistre ce chemin comme le meilleur pour l'instant
                came_from_map[neighbor_pos] = current_node.position;
                g_scores_map[neighbor_pos] = tentative_g_score;

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
