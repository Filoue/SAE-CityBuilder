#include "graphics/building_renderer.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>


BuildingRenderer::BuildingRenderer(){
    for (auto& va : vertex_arrays_) {
        va.setPrimitiveType(sf::PrimitiveType::Triangles);
    }
    textures_.fill(nullptr);
}

void BuildingRenderer::AddBuilding(sf::Vector2f center_pos, float size, BuildingType type, const sf::Texture& texture) {

    textures_[type] = &texture;

    sf::VertexArray& vertices = vertex_arrays_[type];

    float half_size = size / 2.f;

    // 1. Positions dans le monde (Géométrie)
    float left   = center_pos.x - half_size;
    float right  = center_pos.x + half_size;
    float top    = center_pos.y - half_size;
    float bottom = center_pos.y + half_size;

    // 2. Coordonnées de la Texture (en pixels de l'image d'origine)
    sf::Vector2u tex_size = texture.getSize();
    float tex_l = 0.f;
    float tex_r = static_cast<float>(tex_size.x);
    float tex_t = 0.f;
    float tex_b = static_cast<float>(tex_size.y);

    // Couleur blanche par défaut (pour ne pas teinter l'image originale)
    sf::Color white = sf::Color::White;

    // Triangle 1 : Haut-Gauche, Haut-Droite, Bas-Gauche
    vertices.append(sf::Vertex{ .position = {left, top},    .color = white, .texCoords = {tex_l, tex_t} });
    vertices.append(sf::Vertex{ .position = {right, top},   .color = white, .texCoords = {tex_r, tex_t} });
    vertices.append(sf::Vertex{ .position = {left, bottom}, .color = white, .texCoords = {tex_l, tex_b} });

    // Triangle 2 : Bas-Gauche, Haut-Droite, Bas-Droite
    vertices.append(sf::Vertex{ .position = {left, bottom}, .color = white, .texCoords = {tex_l, tex_b} });
    vertices.append(sf::Vertex{ .position = {right, top},   .color = white, .texCoords = {tex_r, tex_t} });
    vertices.append(sf::Vertex{ .position = {right, bottom},.color = white, .texCoords = {tex_r, tex_b} });
}

void BuildingRenderer::Clear() {
    for (auto& va : vertex_arrays_) {
        va.clear();
    }
    textures_.fill(nullptr);
}

void BuildingRenderer::Draw(sf::RenderTarget& target) const {
    for (size_t i = 0; i < BuildingType::kCount; ++i) {
        if (vertex_arrays_[i].getVertexCount() > 0 && textures_[i] != nullptr) {
            sf::RenderStates states;
            states.texture = textures_[i];

            target.draw(vertex_arrays_[i], states);
        }
    }
}