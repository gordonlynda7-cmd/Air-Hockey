#include "Game.h"
#include "Config.h"
#include "raymath.h"
#include <cmath>

namespace {
float aiSpeedFor(int level) {
    switch (level) {
        case 1: return cfg::AI_SPEED_EASY;
        case 3: return cfg::AI_SPEED_HARD;
        default: return cfg::AI_SPEED_MEDIUM;
    }
}
const float MIDDLE = cfg::WIDTH / 2.0f;
const float GOAL_TOP = (cfg::HEIGHT - cfg::GOAL_HEIGHT) / 2.0f;
const float GOAL_BOTTOM = GOAL_TOP + cfg::GOAL_HEIGHT;
}  // namespace

Game::Game() {
    float r = cfg::PADDLE_RADIUS;
    player_ = std::make_unique<HumanPaddle>(Vector2{80.0f, cfg::HEIGHT / 2.0f}, r, MIDDLE - r);
    setDifficulty(difficulty_);
}

void Game::setDifficulty(int level) {
    difficulty_ = level;
    float r = cfg::PADDLE_RADIUS;
    ai_ = std::make_unique<AIPaddle>(Vector2{cfg::WIDTH - 80.0f, cfg::HEIGHT / 2.0f},
                                     MIDDLE + r, cfg::WIDTH - r, aiSpeedFor(level));
}

void Game::startMatch() {
    playerScore_ = 0;
    aiScore_ = 0;
    serveCount_ = 0;
    serve(true);
    state_ = State::Playing;
}

void Game::serve(bool towardPlayer) {
    float vx = towardPlayer ? -260.0f : 260.0f;
    float vy = (serveCount_ % 2 == 0) ? -110.0f : 110.0f;
    serveCount_++;
    puck_.reset(Vector2{cfg::WIDTH / 2.0f, cfg::HEIGHT / 2.0f}, Vector2{vx, vy});
    stuckTimer_ = 0.0f;
    stuckAnchor_ = puck_.position();
    player_->resetToHome();
    ai_->resetToHome();
}

void Game::update(float dt, const Input& in) {
    switch (state_) {
        case State::Title:
            if (in.difficultyKey >= 1 && in.difficultyKey <= 3) setDifficulty(in.difficultyKey);
            player_->update(dt, puck_, in.mouse);
            if (in.confirmPressed) startMatch();
            break;

        case State::Playing:
            stepPlaying(dt, in);
            break;

        case State::GoalPause:
            player_->update(dt, puck_, in.mouse);
            pauseTimer_ -= dt;
            if (pauseTimer_ <= 0.0f) {
                serve(!lastGoalWasPlayers_);  // serve toward whoever just got scored on
                state_ = State::Playing;
            }
            break;

        case State::GameOver:
            if (in.confirmPressed) {
                player_->resetToHome();
                ai_->resetToHome();
                puck_.reset(Vector2{cfg::WIDTH / 2.0f, cfg::HEIGHT / 2.0f}, Vector2{0.0f, 0.0f});
                state_ = State::Title;
            }
            break;
    }
}

void Game::stepPlaying(float dt, const Input& in) {
    float h = dt / cfg::PHYSICS_SUBSTEPS;
    for (int i = 0; i < cfg::PHYSICS_SUBSTEPS; ++i) {
        player_->update(h, puck_, in.mouse);
        ai_->update(h, puck_, in.mouse);
        puck_.update(h);
        collidePuckWithPaddle(*player_);
        collidePuckWithPaddle(*ai_);
        handleWalls();
        if (state_ != State::Playing) return;  // a goal was scored
    }
    checkForStuckPuck(dt);
}

// A puck can get pinned between a paddle and a wall or corner. Real tables have the same
// problem, so if the puck has not really moved for a while we serve it again.
void Game::checkForStuckPuck(float dt) {
    if (Vector2Distance(puck_.position(), stuckAnchor_) > cfg::STUCK_DISTANCE) {
        stuckAnchor_ = puck_.position();
        stuckTimer_ = 0.0f;
        return;
    }
    stuckTimer_ += dt;
    if (stuckTimer_ > cfg::STUCK_SECONDS) {
        serve(puck_.position().x < MIDDLE);  // serve toward the side it was stuck on
    }
}

void Game::collidePuckWithPaddle(const Paddle& paddle) {
    Vector2 offset = Vector2Subtract(puck_.position(), paddle.position());
    float distance = Vector2Length(offset);
    float touching = puck_.radius() + paddle.radius();
    if (distance >= touching) return;  // not touching

    // The collision "normal": the direction pointing from the paddle to the puck.
    Vector2 normal = distance > 0.0001f ? Vector2Scale(offset, 1.0f / distance) : Vector2{1.0f, 0.0f};

    // 1. Push the puck out so it is exactly touching, not overlapping.
    puck_.setPosition(Vector2Add(paddle.position(), Vector2Scale(normal, touching)));

    // 2. Bounce. Work in the paddle's frame of reference so a moving paddle hits harder.
    Vector2 relative = Vector2Subtract(puck_.velocity(), paddle.velocity());
    float approach = Vector2DotProduct(relative, normal);  // negative means moving toward the paddle
    if (approach < 0.0f) {
        relative = Vector2Subtract(relative, Vector2Scale(normal, (1.0f + cfg::PADDLE_HIT_BOUNCE) * approach));
        puck_.setVelocity(Vector2Add(relative, paddle.velocity()));
    }
}

void Game::handleWalls() {
    Vector2 p = puck_.position();
    Vector2 v = puck_.velocity();
    float r = puck_.radius();

    // Top and bottom walls: always solid.
    if (p.y - r < 0.0f) { p.y = r; v.y = std::fabs(v.y) * cfg::WALL_BOUNCE; }
    if (p.y + r > cfg::HEIGHT) { p.y = cfg::HEIGHT - r; v.y = -std::fabs(v.y) * cfg::WALL_BOUNCE; }

    // Left and right walls: solid except for the goal opening.
    bool inGoalOpening = p.y > GOAL_TOP && p.y < GOAL_BOTTOM;

    if (inGoalOpening && p.x + r < 0.0f) { scoreGoal(false); return; }               // AI scored
    if (inGoalOpening && p.x - r > cfg::WIDTH) { scoreGoal(true); return; }          // player scored

    if (!inGoalOpening && p.x - r < 0.0f) { p.x = r; v.x = std::fabs(v.x) * cfg::WALL_BOUNCE; }
    if (!inGoalOpening && p.x + r > cfg::WIDTH) { p.x = cfg::WIDTH - r; v.x = -std::fabs(v.x) * cfg::WALL_BOUNCE; }

    puck_.setPosition(p);
    puck_.setVelocity(v);
}

void Game::scoreGoal(bool playerScored) {
    lastGoalWasPlayers_ = playerScored;
    if (playerScored) playerScore_++; else aiScore_++;
    puck_.setVelocity(Vector2{0.0f, 0.0f});

    if (playerScore_ >= cfg::WIN_SCORE || aiScore_ >= cfg::WIN_SCORE) {
        state_ = State::GameOver;
    } else {
        state_ = State::GoalPause;
        pauseTimer_ = cfg::GOAL_PAUSE_SECONDS;
    }
}
