#pragma once
#include "raylib.h"
#include "Puck.h"

// Paddle is an abstract base class. A HumanPaddle and an AIPaddle are both paddles,
// but they decide where to move in different ways. That decision is the only thing
// that differs, so it is the one virtual function (polymorphism).
class Paddle {
public:
    Paddle(Vector2 home, float minX, float maxX);
    virtual ~Paddle() = default;

    // Called every physics step. Each subclass picks a target and calls moveToward().
    virtual void update(float dt, const Puck& puck, Vector2 mouse) = 0;

    void resetToHome();

    Vector2 position() const { return position_; }
    Vector2 velocity() const { return velocity_; }  // the Game uses this to give the puck a kick
    float radius() const;

protected:
    // Slide toward target without going faster than maxSpeed or leaving this paddle's half of the table.
    void moveToward(Vector2 target, float maxSpeed, float dt);

    Vector2 position_;
    Vector2 velocity_;
    Vector2 home_;
    float minX_;
    float maxX_;
};

// Follows the mouse.
class HumanPaddle : public Paddle {
public:
    using Paddle::Paddle;
    void update(float dt, const Puck& puck, Vector2 mouse) override;
};

// Chases the puck when it is on its side, and drifts back home when it is not.
class AIPaddle : public Paddle {
public:
    AIPaddle(Vector2 home, float minX, float maxX, float maxSpeed);
    void update(float dt, const Puck& puck, Vector2 mouse) override;

private:
    float maxSpeed_;  // the difficulty setting: a slower AI is easier to beat
};
