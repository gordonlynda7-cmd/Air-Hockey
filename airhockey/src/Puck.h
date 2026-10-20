#pragma once
#include "raylib.h"

// The puck: knows where it is and how fast it is moving. It does not know about
// walls, paddles, or goals. The Game class handles those, so each class has one job.
class Puck {
public:
    Puck();

    void reset(Vector2 position, Vector2 velocity);
    void update(float dt);  // move, apply ice friction, cap the speed

    Vector2 position() const { return position_; }
    Vector2 velocity() const { return velocity_; }
    float radius() const;

    void setPosition(Vector2 p) { position_ = p; }
    void setVelocity(Vector2 v) { velocity_ = v; }

private:
    Vector2 position_;
    Vector2 velocity_;
};
