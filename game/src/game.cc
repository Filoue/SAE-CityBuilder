#include <optional>
#include <filesystem>
#include "SFML/Graphics.hpp"
#include "game.h"

#include "tilemap.h"
#include "ai/normalize_npc.h"
#include "ai/npc_manager.h"
#include "graphics/camera.h"
#include "SaveSystem/SaveManager.h"
#include "editMode/edit_mod.h"
#include "worldSettings/world_settings.h"
#include "resource_manager.h"

namespace game {
    namespace {
        constexpr sf::Vector2f window_size_f = {
            static_cast<float>(api::tiles::WorldSettings::nb_tiles.x * api::tiles::WorldSettings::tile_size.x),
            static_cast<float>(api::tiles::WorldSettings::nb_tiles.y * api::tiles::WorldSettings::tile_size.y)
        };
        constexpr sf::Vector2u window_size_u = {
            static_cast<unsigned>(api::tiles::WorldSettings::nb_tiles.x * api::tiles::WorldSettings::tile_size.x),
            static_cast<unsigned>(api::tiles::WorldSettings::nb_tiles.y * api::tiles::WorldSettings::tile_size.y)
        };

        sf::Clock clock_;
        sf::RenderWindow window_;
        bool isFullscreen_ = false;

        EditMode edit_mode;
        ResourceManager resource_manager;

        Tilemap map_;
        graphics::Camera camera_;

        SaveManager save("save.json");

        void Setup(){
            // Create the main window
            window_.create(sf::VideoMode::getDesktopMode(), "SFML window", sf::State::Fullscreen);
            camera_.Setup(window_size_f);
            map_.Setup(api::tiles::WorldSettings::nb_tiles, sf::Vector2f(api::tiles::WorldSettings::tile_size));
            api::ai::NpcManager::GetInstance().SetupManager(1, map_, resource_manager, {5, 5});
            edit_mode.Setup(api::tiles::WorldSettings::tile_size.x, sf::Vector2f(api::tiles::WorldSettings::tile_size), map_, resource_manager);
        
            // Add initial resources
            resource_manager.AddWood(5);
            resource_manager.AddStone(10);

            // Add initial houses
            edit_mode.buildings_.push_back({{5, 5}, Housing::kWoodCutterHouse});
            edit_mode.buildings_.push_back({{10, 10}, Housing::kMinerHouse});
            edit_mode.Building();

            save.Set("Tilemap", map_);
            save.Set("edit_mode", edit_mode);
            save.Set("ressource", resource_manager);
        }

        void ToggleFullscreen(){
            isFullscreen_ = !isFullscreen_;
            if (isFullscreen_) {
                window_.create(sf::VideoMode::getDesktopMode(), "SFML window",
                               sf::State::Fullscreen);
            } else {
                window_.create(sf::VideoMode(window_size_u), "SFML window",
                               sf::Style::Default);
            }
            camera_.OnWindowResized(window_.getSize());
        }
    } // namespace

    void Loop(){

        if (std::filesystem::exists("save.json")) {
            save.LoadFromFile("save.json");
        }else {
            Setup();
        }

        // Start the game loop
        while (window_.isOpen()) {
            const float dt = clock_.restart().asSeconds();

            // Process events = Input frame
            while (const std::optional event = window_.pollEvent()) {
                // Close window: exit
                if (event->is<sf::Event::Closed>()) {
                    window_.close();
                }
                if (const auto *key = event->getIf<sf::Event::KeyPressed>()) {
                    if (key->code == sf::Keyboard::Key::Enter && key->alt) {
                        ToggleFullscreen();
                        continue;
                    }
                    if (key->code == sf::Keyboard::Key::Escape) {
                        window_.close();
                    }

                    if (key->code == sf::Keyboard::Key::Num1) {
                        api::ai::NpcManager::GetInstance().AddNpc(1, map_, Housing::kWoodCutterHouse, {0, 0});
                    }
                    if (key->code == sf::Keyboard::Key::E) {
                        edit_mode.Toggle();
                    }
                }
                camera_.HandleEvent(*event, window_);
                edit_mode.HandleEvent(*event, window_);
            }

            camera_.Update(dt);
            camera_.Apply(window_);
            api::ai::NpcManager::GetInstance().Update(dt);
            edit_mode.Update(window_);

            // Graphic frame
            window_.clear();
            map_.Draw(window_);
            api::ai::NpcManager::GetInstance().Draw(window_);
            edit_mode.Draw(window_);
            window_.display();
        }
    }
} // namespace game
