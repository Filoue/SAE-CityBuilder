//
// Created by petitfiloue on 19.06.2026.
//
#include "editMode/edit_mod.h"
#include "tilemap.h"
#include <cmath>
#include <expected>

#include "tiles/tilemap_generator.h"
// Ton nouveau renderer
#include "graphics/building_renderer.h"

void EditMode::Setup(float tile_size, sf::Vector2f grid_offset, Tilemap& tilemap) {
    tile_size_ = tile_size;
    grid_offset_ = grid_offset;
    tilemap_ = &tilemap;
    housing_ = Housing::kNone;

    hunterHouse_ = std::make_unique<sf::Texture>();
    minerHouse_ = std::make_unique<sf::Texture>();
    woodCutterHouse_ = std::make_unique<sf::Texture>();

    hunterHouse_->loadFromFile("_assets/kenney_medieval-rts/PNG/Default size/Structure/medievalStructure_01.png");
    minerHouse_->loadFromFile("_assets/kenney_medieval-rts/PNG/Default size/Structure/medievalStructure_02.png");
    woodCutterHouse_->loadFromFile("_assets/kenney_medieval-rts/PNG/Default size/Structure/medievalStructure_03.png");
}

void EditMode::HandleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (!enabled_) return;
    if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseButton->button == sf::Mouse::Button::Left) {
            TerrainTiles tile = tilemap_->GetTerrainTileType(hovered_grid_pos_);
            if (Placable(tile)) {
                bool occupied = false;

                // Ton TODO est une excellente idée pour la suite (unordered_set)
                for (const auto& b : buildings_) {
                    if (b.gridPos == hovered_grid_pos_) { occupied = true; break; }
                }

                if (!occupied) {
                    buildings_.push_back({hovered_grid_pos_, current_selection_});
                    // On reconstruit les vertices seulement quand la liste change !
                    Building();
                }
            }
        }
    }
    if (const auto* keyPad = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPad->code == sf::Keyboard::Key::Numpad1) selection = 0;
        if (keyPad->code == sf::Keyboard::Key::Numpad2) selection = 1;
        if (keyPad->code == sf::Keyboard::Key::Numpad3) selection = 2;
    }
    switch (selection) {
        case 0:
            current_selection_ = Housing::kHunterHouse;
            color_ = sf::Color(0,0,255);
            housing_ = current_selection_;
            break;
        case 1:
            current_selection_ = Housing::kMinerHouse;
            color_ = sf::Color(100,100,100);
            housing_ = current_selection_;
            break;
        case 2:
            current_selection_ = Housing::kWoodCutterHouse;
            color_ = sf::Color(179, 79, 50);
            housing_ = current_selection_;
            break;
    }
}

void EditMode::Update(const sf::RenderWindow& window) {
    if (!enabled_) return;

    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

    hovered_grid_pos_ = WorldToGrid(worldPos);
}

// Cette méthode reconstruit le VertexArray à partir du vecteur de structures 'buildings_'
void EditMode::Building() {
    // Remplace 'buildinglist_' par ton instance de classe (ex: building_renderer_)
    building_renderer_.Clear();

    for (const auto& b : buildings_) {
        sf::Vector2f pos = GridToWorld(b.gridPos);
        float size = tile_size_;

        const sf::Texture* buildingTexture = nullptr;
        BuildingRenderer::BuildingType typeIndex;

        // On récupère la couleur adéquate selon le type de bâtiment stocké
        // Note: Ajuste 'b.type' selon le nom exact de la variable dans ta structure Building
        switch (b.type) {
            case Housing::kHunterHouse:
                buildingTexture = hunterHouse_.get();
                typeIndex = BuildingRenderer::kHunter;
                break;
            case Housing::kMinerHouse:
                buildingTexture = minerHouse_.get();
                typeIndex = BuildingRenderer::kMiner;
                break;
            case Housing::kWoodCutterHouse:
                buildingTexture = woodCutterHouse_.get();
                typeIndex = BuildingRenderer::kWoodCutter;
                break;
            default:                        break;
        }
        if (buildingTexture != nullptr) {
            building_renderer_.AddBuilding(pos, size, typeIndex, *buildingTexture);
        }
    }
}

void EditMode::Draw(sf::RenderWindow& window) {
    // 1. Dessiner tous les bâtiments d'un seul coup !
    building_renderer_.Draw(window);

    TerrainTiles tile = tilemap_->GetTerrainTileType(hovered_grid_pos_);

    // 2. Dessiner le bâtiment "Fantôme" au curseur
    if (enabled_) {
        sf::RectangleShape ghost(sf::Vector2f(tile_size_, tile_size_));
        ghost.setOrigin(ghost.getSize() / 2.f);
        ghost.setPosition(GridToWorld(hovered_grid_pos_));

        if (Placable(tile)) {
            // 1. On récupère le pointeur brut de la texture
            const sf::Texture* textureToUse = nullptr;

            switch (housing_) {
                case Housing::kHunterHouse:     textureToUse = hunterHouse_.get(); break;
                case Housing::kMinerHouse:      textureToUse = minerHouse_.get(); break;
                case Housing::kWoodCutterHouse: textureToUse = woodCutterHouse_.get(); break;
                default: break;
            }

            // 2. Si la texture est valide, on instancie et on dessine le sprite ICI
            if (textureToUse != nullptr) {
                sf::Sprite houseSprite(*textureToUse); // Déclarée localement à l'intérieur du bloc
                houseSprite.setColor(sf::Color(255, 255, 255, 150));
                houseSprite.setPosition(ghost.getPosition());

                window.draw(houseSprite);
            }
        }
        else {
            // Si ce n'est pas plaçable, on affiche le rectangle rouge transparent
            ghost.setFillColor(sf::Color(255, 0, 0, 127));
            window.draw(ghost);
        }
    }
}


sf::Vector2i EditMode::WorldToGrid(sf::Vector2f world_pos) const {
    float adjusted_x = world_pos.x - grid_offset_.x;
    float adjusted_y = world_pos.y - grid_offset_.y;
    return sf::Vector2i(static_cast<int>(std::floor(adjusted_x / tile_size_)),
                        static_cast<int>(std::floor(adjusted_y / tile_size_)));
}

sf::Vector2f EditMode::GridToWorld(sf::Vector2i grid_pos) const {
    return sf::Vector2f(static_cast<float>(grid_pos.x) * tile_size_ + tile_size_ / 2.f,
                        static_cast<float>(grid_pos.y) * tile_size_ + tile_size_ / 2.f) + grid_offset_;
}

bool EditMode::Placable(TerrainTiles tile) {
    switch (tile) {
        case TerrainTiles::kForest:
        case TerrainTiles::kGrassA:
        case TerrainTiles::kGrassB:
        case TerrainTiles::kSandA:
        case TerrainTiles::kSandB:
            return true;
        case TerrainTiles::kWaterA:
        case TerrainTiles::kWaterB:
            return false;
        default:
            return false;
    }
}