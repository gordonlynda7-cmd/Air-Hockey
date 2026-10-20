#include "Puck.h"
#include "Config.h"
#include "raymath.h"

Puck::Puck() : position_{cfg::WIDTH / 2.0f, cfg::HEIGHT / 2.0f}, velocity_{0.0f, 0.0f} {}

void Puck::reset(Vector2 position, Vector2 velocity) {
    position_ = position;
    velocity_ = velocity;
}

float Puck::radius() const { return cfg::PUCK_RADIUS; }

void Puck::update(float dt) {
    position_ = Vector2Add(position_, Vector2Scale(velocity_, dt));

    float keep = 1.0f - cfg::PUCK_DAMPING * dt;
    if (keep < 0.0f) keep = 0.0f;
    velocity_ = Vector2Scale(velocity_, keep);

    float speed = Vector2Length(velocity_);
    if (speed > cfg::PUCK_MAX_SPEED) {
        velocity_ = Vector2Scale(velocity_, cfg::PUCK_MAX_SPEED / speed);
    }
}
