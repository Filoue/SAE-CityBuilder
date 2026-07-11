
#ifndef CITYBUILDER_BUILDING_RENDERER_H
#define CITYBUILDER_BUILDING_RENDERER_H


#include <SFML/Graphics.hpp>

class BuildingRenderer {
public:

    enum BuildingType {
        kHunter = 0,
        kMiner,
        kWoodCutter,
        kCount
    };

    BuildingRenderer();

    // Ajoute un bâtiment (génère 2 triangles / 6 vertices)
    void AddBuilding(sf::Vector2f center_pos, float size,BuildingType type, const sf::Texture& texture);

    // Vide le tableau de vertices pour reconstruction
    void Clear();

    // Dessine tous les bâtiments d'un coup
    void Draw(sf::RenderTarget& target) const;

private:

    std::array<sf::VertexArray, BuildingType::kCount> vertex_arrays_;
    std::array<const sf::Texture*, BuildingType::kCount> textures_;

};
#endif
