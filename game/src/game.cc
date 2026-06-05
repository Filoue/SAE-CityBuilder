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
        constexpr sf::Vector2f world_size = {1920.f * 5, 1080.f * 5};
        constexpr sf::Vector2f window_size_f = {1920.f, 1080.f};
        constexpr sf::Vector2u window_size_u = {1920u, 1080u};

        sf::Clock clock_;
        sf::RenderWindow window_;
        bool isFullscreen_ = false;

        api::ai::NpcManager npc_manager;

        Tilemap map_;
        graphics::Camera camera_;

        void Setup(){
            // Create the main window
            window_.create(sf::VideoMode(window_size_u), "SFML window", sf::State::Fullscreen);
            camera_.Setup(window_size_f);
            map_.Setup(world_size, {32, 32});
            npc_manager.SetupManager(10, world_size);
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

            npc_manager.Update(dt);

            // Graphic frame
            window_.clear();
            map_.Draw(window_);
            npc_manager.Draw(window_);
            window_.display();
        }
    }
} // namespace game
