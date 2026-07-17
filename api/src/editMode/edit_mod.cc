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
#include "ai/npc_manager.h"

void EditMode::Setup(float tile_size, sf::Vector2f grid_offset, Tilemap& tilemap, ResourceManager& resource_manager) {
    tile_size_ = tile_size;
    grid_offset_ = grid_offset;
    tilemap_ = &tilemap;
    resource_manager_ = &resource_manager;
    housing_ = Housing::kNone;

    hunterHouse_ = std::make_unique<sf::Texture>();
    minerHouse_ = std::make_unique<sf::Texture>();
    woodCutterHouse_ = std::make_unique<sf::Texture>();

    hunterHouse_->loadFromFile("_assets/kenney_medieval-rts/PNG/Default size/Structure/medievalStructure_03.png");
    minerHouse_->loadFromFile("_assets/kenney_medieval-rts/PNG/Default size/Structure/medievalStructure_01.png");
    woodCutterHouse_->loadFromFile("_assets/kenney_medieval-rts/PNG/Default size/Structure/medievalStructure_02.png");
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
                    int wood_cost = 0;
                    int stone_cost = 0;
                    switch (current_selection_) {
                        case Housing::kHunterHouse:
                            wood_cost = 100;
                            stone_cost = 500;
                            break;
                        case Housing::kMinerHouse:
                            wood_cost = 5;
                            stone_cost = 10;
                            break;
                        case Housing::kWoodCutterHouse:
                            wood_cost = 10;
                            stone_cost = 5;
                            break;
                        default:
                            break;
                    }

                    if (resource_manager_->HasEnoughResources(wood_cost, stone_cost)) {
                        resource_manager_->SpendResources(wood_cost, stone_cost);
                        buildings_.push_back({hovered_grid_pos_, current_selection_});
                        // On reconstruit les vertices seulement quand la liste change !
                        Building();
                        api::ai::NpcManager::GetInstance().AddNpc(1, *tilemap_, current_selection_, hovered_grid_pos_);
                    }
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

    const sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    const sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

    if (const auto idx = api::tiles::WorldSettings::PixelPosToIdx(worldPos)) {
        hovered_grid_pos_ = api::tiles::WorldSettings::IdxToTilePos(*idx);
    }
}

// Cette méthode reconstruit le VertexArray à partir du vecteur de structures 'buildings_'
void EditMode::Building() {
    // Remplace 'buildinglist_' par ton instance de classe (ex: building_renderer_)
    building_renderer_.Clear();

    for (const auto& b : buildings_) {
        const auto idx = api::tiles::WorldSettings::TilePosToIdx(b.gridPos);
        const sf::Vector2f pos = sf::Vector2f{api::tiles::WorldSettings::IdxToPixelPos(idx)} + sf::Vector2f{api::tiles::WorldSettings::tile_size.x / 2.f, api::tiles::WorldSettings::tile_size.y / 2.f};
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
        const auto idx = api::tiles::WorldSettings::TilePosToIdx(hovered_grid_pos_);
        ghost.setPosition(sf::Vector2f{api::tiles::WorldSettings::IdxToPixelPos(idx)} + sf::Vector2f{api::tiles::WorldSettings::tile_size.x / 2.f, api::tiles::WorldSettings::tile_size.y / 2.f});

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
