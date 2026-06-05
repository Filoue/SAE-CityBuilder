#ifndef API_MOTION_MOTOR_H
#define API_MOTION_MOTOR_H

#include <SFML/System/Vector2.hpp>

namespace api::motion {

// Tiny linear mover: each Update() steps position_ toward destination_ at a
// fixed speed, snapping onto the destination once it is within reach.
// Provided to students as-is for the rough NPC (no pathfinding).
class Motor {
 public:
  void Update(float dt);  // call once per frame
  [[nodiscard]] float RemainingDistance() const;

  void SetSpeed(float speed);
  void SetPosition(sf::Vector2f position);
  void SetDestination(sf::Vector2f destination);

  [[nodiscard]] const sf::Vector2f& GetPosition() const;

 private:
  sf::Vector2f position_;
  sf::Vector2f destination_;
  float speed_ = 0.f;
  float remainingDistance_ = 0.f;
};

}  // namespace api::motion

#endif  // API_MOTION_MOTOR_H
