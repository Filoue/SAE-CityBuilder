//
// Created by petit on 04.06.2026.
//

#include "ai/normalize_npc.h"

#include <print>

#include "ai/bt_node_factory.h"
#include "ai/bt_action.h"
#include "ai/bt_composite.h"
#include "ai/bt_sequence.h"
#include "ai/bt_selector.h"


namespace api::ai {
    using core::ai::behaviour_tree::Status;


    void NormalizeNpc::Setup(std::string_view sprite_path, sf::Vector2f world_size, sf::Vector2f start_position){
        world_size_ = world_size;

        if (texture_->loadFromFile(std::string(sprite_path))) {
            sprite_ = sf::Sprite(*texture_);
        }

        motor_.SetPosition(start_position);
        motor_.SetDestination(start_position);
        motor_.SetSpeed(kSpeed);

        using namespace core::ai::behaviour_tree;
        using namespace node_factory;

        auto move = MakeAction([this]{return MoveToDestination();});
        auto pick = MakeAction([this]{return PickRandomDestination();});

        auto wanderSequence = std::make_unique<SequenceNode>();
        wanderSequence->AddChild(std::move(pick));
        wanderSequence->AddChild(std::move(move));

        bt_root_ = std::move(wanderSequence);

    }
    void NormalizeNpc::Update(float dt){
        motor_.Update(dt);
        if (bt_root_) bt_root_->Tick();
    }
    void NormalizeNpc::Draw(sf::RenderWindow &window){
        if (sprite_.has_value()) {
            sprite_->setPosition(motor_.GetPosition());
            window.draw(*sprite_);
        }
    }

    Status NormalizeNpc::PickRandomDestination(){
        std::uniform_real_distribution<float> x_dist(0.f, world_size_.x);
        std::uniform_real_distribution<float> y_dist(0.f, world_size_.y);
        motor_.SetDestination({x_dist(rng_), y_dist(rng_)});
        return  Status::kSuccess;
    }

    Status NormalizeNpc::MoveToDestination() const {
        return motor_.RemainingDistance() <= 0.001f ? Status::kSuccess : Status::kRunning;
    }

    Status NormalizeNpc::Locked() {
        std::println("Locked");
        return Status::kSuccess;
    }

}
