#include "motion/motor.h"

namespace api::motion {

void Motor::Update(const float dt) {
  const sf::Vector2f distance = destination_ - position_;
  remainingDistance_ = distance.length();

  if (remainingDistance_ < speed_ * dt) {
    position_ = destination_;
    remainingDistance_ = 0.f;
    return;
  }
  position_ += distance.normalized() * speed_ * dt;
}

float Motor::RemainingDistance() const { return remainingDistance_; }

void Motor::SetSpeed(float speed) { speed_ = speed; }

void Motor::SetPosition(sf::Vector2f position) {
  position_ = position;
  remainingDistance_ = (destination_ - position_).length();
}

void Motor::SetDestination(sf::Vector2f destination) {
  destination_ = destination;
  remainingDistance_ = (destination_ - position_).length();
}

const sf::Vector2f& Motor::GetPosition() const { return position_; }

}  // namespace api::motion
