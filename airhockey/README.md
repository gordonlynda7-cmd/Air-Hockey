# Air Hockey

A two-player-style air hockey game against a computer opponent, written in C++17 with [raylib](https://www.raylib.com/).
Move the mouse to control the blue paddle and score 7 goals before the CPU does.

## Run it (Mac)

```
brew install raylib pkg-config
make play
```

`make test` runs the game-logic tests (no window needed). `make clean` removes build files.

## Controls

| Key | Action |
|-----|--------|
| Mouse | Move your paddle (it stays on the left half) |
| 1 / 2 / 3 | Pick Easy / Medium / Hard on the title screen |
| SPACE | Start, and play again after a match |
| ESC | Quit |

## How it is organized

```
Game      owns the rules: states, scoring, collisions, goals
 |-- Puck             position and velocity, ice friction, speed cap
 |-- Paddle (abstract base class)
 |     |-- HumanPaddle    follows the mouse
 |     `-- AIPaddle       chases the puck and aims at your goal
Renderer  draws a Game (read-only, never changes it)
Config.h  every tunable number in one place
```

- **Polymorphism:** `Game` holds both paddles as `Paddle` pointers and calls `update()` on each. Only the
  decision about where to move differs between the human and the AI.
- **Game loop:** each frame reads input, advances physics in 4 small substeps so a fast puck can never skip
  through a paddle, then draws.
- **Collisions:** circle against circle. The puck is pushed out of the overlap, then bounced using the paddle's
  own velocity, so a moving paddle hits harder than a still one.
- **Testable on purpose:** input goes in through a plain `Input` struct and drawing lives in `Renderer`, so the
  rules in `Game` run in tests with no window. See `tests/test_game.cpp`.
- **Stuck puck:** if the puck gets pinned against a wall or corner and barely moves for 1.5 seconds, it is
  served again, the same way a real table handles it.

## Ideas to extend it

- Sound effects on hits and goals
- Two-player mode (second paddle on the keyboard)
- Power-ups or a trail effect on a fast puck
- Smarter AI that predicts bounces off the walls
