#pragma once
// All the tunable numbers in one place. Change these to change how the game feels.
namespace cfg {
constexpr int   WIDTH = 900;
constexpr int   HEIGHT = 500;

constexpr float PUCK_RADIUS = 16.0f;
constexpr float PADDLE_RADIUS = 34.0f;

constexpr float GOAL_HEIGHT = 170.0f;        // size of the goal opening in each side wall
constexpr float PUCK_MAX_SPEED = 950.0f;     // pixels per second
constexpr float PUCK_DAMPING = 0.25f;        // fraction of speed lost per second (ice friction)
constexpr float WALL_BOUNCE = 0.92f;         // 1.0 would be a perfect bounce
constexpr float PADDLE_HIT_BOUNCE = 0.95f;

constexpr float PLAYER_MAX_SPEED = 1500.0f;  // how fast the player's paddle can chase the mouse
constexpr float AI_SPEED_EASY = 330.0f;
constexpr float AI_SPEED_MEDIUM = 520.0f;
constexpr float AI_SPEED_HARD = 760.0f;

constexpr int   WIN_SCORE = 7;
constexpr float GOAL_PAUSE_SECONDS = 1.2f;
constexpr float STUCK_SECONDS = 1.5f;       // puck barely moved for this long -> re-serve
constexpr float STUCK_DISTANCE = 6.0f;       // ..."barely moved" means less than this many pixels
constexpr int   PHYSICS_SUBSTEPS = 4;        // small steps each frame so fast pucks never skip through things
}
