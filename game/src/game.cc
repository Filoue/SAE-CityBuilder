#include <optional>
#include "SFML/Graphics.hpp"
#include "game.h"

#include "tilemap.h"
#include "ai/normalize_npc.h"
#include "ai/npc_manager.h"
#include "graphics/camera.h"
#include "graphics/tilemap_renderer.h"
#include "graphics/tilesheet.h"

namespace game {
    namespace {
        constexpr sf::Vector2i world_grid_dimensions = {120, 68};
        constexpr sf::Vector2f window_size_f = {1920.f, 1080.f};
        constexpr sf::Vector2u window_size_u = {1920u, 1080u};

        sf::Clock clock_;
        sf::RenderWindow window_;
        bool isFullscreen_ = false;

        api::ai::NpcManager npc_manager;
        api::ai::NormalizeNpc npc_;

        Tilemap map_;
        graphics::Camera camera_;

        void Setup(){
            // Create the main window
            window_.create(sf::VideoMode(window_size_u), "SFML window", sf::State::Fullscreen);
            camera_.Setup(window_size_f);
            map_.Setup(world_grid_dimensions, {32.f, 32.f});
            //npc_.Setup("_assets/kenney_medieval-rts/PNG/Default size/Unit/medievalUnit_01.png", world_grid_dimensions, {0,0}, map_, 32.f, {0.f, 0.f});
            npc_manager.SetupManager(3000, world_grid_dimensions, map_, 32, {0.f, 0.f});
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

        Setup();

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
                }
                camera_.HandleEvent(*event, window_);
            }

            camera_.Update(dt);
            camera_.Apply(window_);
            //npc_.Update(dt);
            npc_manager.Update(dt);

            // Graphic frame
            window_.clear();
            map_.Draw(window_);
            npc_.Draw(window_);
            npc_manager.Draw(window_);
            window_.display();
        }
    }
} // namespace game
