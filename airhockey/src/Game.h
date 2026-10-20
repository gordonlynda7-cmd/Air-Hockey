#pragma once
#include <memory>
#include "raylib.h"
#include "Puck.h"
#include "Paddle.h"

enum class State { Title, Playing, GoalPause, GameOver };

// Everything the game needs from the outside world for one frame.
// Keeping input in a plain struct means the game logic never calls raylib
// input functions, so it can be tested without opening a window.
struct Input {
    Vector2 mouse{80.0f, 250.0f};
    bool confirmPressed = false;  // SPACE
    int difficultyKey = 0;        // 1, 2, or 3 on the title screen; 0 when none pressed
};

class Game {
public:
    Game();

    void update(float dt, const Input& in);

    // Read-only view of the game, for the Renderer.
    State state() const { return state_; }
    const Puck& puck() const { return puck_; }
    const Paddle& player() const { return *player_; }
    const Paddle& ai() const { return *ai_; }
    int playerScore() const { return playerScore_; }
    int aiScore() const { return aiScore_; }
    int difficulty() const { return difficulty_; }
    bool lastGoalWasPlayers() const { return lastGoalWasPlayers_; }
    bool playerWon() const { return playerScore_ > aiScore_; }

    // Used by the tests to set up exact situations.
    void debugPlacePuck(Vector2 position, Vector2 velocity) { puck_.reset(position, velocity); }

private:
    void startMatch();
    void serve(bool towardPlayer);
    void setDifficulty(int level);
    void stepPlaying(float dt, const Input& in);
    void collidePuckWithPaddle(const Paddle& paddle);
    void handleWalls();       // bounces off walls and detects goals
    void checkForStuckPuck(float dt);
    void scoreGoal(bool playerScored);

    Puck puck_;
    std::unique_ptr<Paddle> player_;
    std::unique_ptr<Paddle> ai_;
    State state_ = State::Title;
    int playerScore_ = 0;
    int aiScore_ = 0;
    int difficulty_ = 2;
    int serveCount_ = 0;
    float pauseTimer_ = 0.0f;
    float stuckTimer_ = 0.0f;       // how long the puck has been (almost) still
    Vector2 stuckAnchor_{0.0f, 0.0f};
    bool lastGoalWasPlayers_ = false;
};
