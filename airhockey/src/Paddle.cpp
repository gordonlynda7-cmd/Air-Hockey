#include "Paddle.h"
#include "Config.h"
#include "raymath.h"

Paddle::Paddle(Vector2 home, float minX, float maxX)
    : position_(home), velocity_{0.0f, 0.0f}, home_(home), minX_(minX), maxX_(maxX) {}

float Paddle::radius() const { return cfg::PADDLE_RADIUS; }

void Paddle::resetToHome() {
    position_ = home_;
    velocity_ = Vector2{0.0f, 0.0f};
}

void Paddle::moveToward(Vector2 target, float maxSpeed, float dt) {
    target.x = Clamp(target.x, minX_, maxX_);
    target.y = Clamp(target.y, radius(), cfg::HEIGHT - radius());

    Vector2 toTarget = Vector2Subtract(target, position_);
    float distance = Vector2Length(toTarget);
    float maxStep = maxSpeed * dt;

    Vector2 step = toTarget;
    if (distance > maxStep && distance > 0.0f) {
        step = Vector2Scale(toTarget, maxStep / distance);
    }

    position_ = Vector2Add(position_, step);
    velocity_ = dt > 0.0f ? Vector2Scale(step, 1.0f / dt) : Vector2{0.0f, 0.0f};
}

void HumanPaddle::update(float dt, const Puck&, Vector2 mouse) {
    moveToward(mouse, cfg::PLAYER_MAX_SPEED, dt);
}

AIPaddle::AIPaddle(Vector2 home, float minX, float maxX, float maxSpeed)
    : Paddle(home, minX, maxX), maxSpeed_(maxSpeed) {}

void AIPaddle::update(float dt, const Puck& puck, Vector2) {
    Vector2 p = puck.position();
    bool puckOnMySide = p.x > cfg::WIDTH / 2.0f;

    Vector2 target;
    if (puckOnMySide) {
        // Stand on the far side of the puck from the player's goal, on the straight line
        // between the two, so pushing the puck sends it toward that goal.
        Vector2 playerGoal{0.0f, cfg::HEIGHT / 2.0f};
        Vector2 aim = Vector2Normalize(Vector2Subtract(p, playerGoal));
        float standOff = (radius() + puck.radius()) * 0.8f;
        target = Vector2Add(p, Vector2Scale(aim, standOff));
    } else {
        // Wait near home, leaning toward where the puck is so we are ready.
        target = Vector2{home_.x, Lerp(home_.y, p.y, 0.5f)};
    }
    moveToward(target, maxSpeed_, dt);
}
