#include "Renderer.h"
#include "Config.h"
#include <cstdio>

namespace {
const Color ICE = {232, 244, 252, 255};
const Color LINE_BLUE = {120, 170, 220, 255};
const Color PLAYER_COLOR = {40, 110, 220, 255};
const Color AI_COLOR = {220, 60, 60, 255};

void drawCentered(const char* text, int y, int size, Color color) {
    int width = MeasureText(text, size);
    DrawText(text, (cfg::WIDTH - width) / 2, y, size, color);
}

void drawPaddle(const Paddle& paddle, Color color) {
    DrawCircleV(paddle.position(), paddle.radius(), color);
    DrawCircleV(paddle.position(), paddle.radius() * 0.55f, Fade(WHITE, 0.35f));
    DrawCircleLines((int)paddle.position().x, (int)paddle.position().y, paddle.radius(), Fade(BLACK, 0.5f));
}
}  // namespace

void Renderer::draw(const Game& game) const {
    ClearBackground(ICE);
    drawRink();

    drawPaddle(game.player(), PLAYER_COLOR);
    drawPaddle(game.ai(), AI_COLOR);

    Vector2 puck = game.puck().position();
    DrawCircleV(puck, game.puck().radius(), Color{30, 30, 40, 255});
    DrawCircleV(Vector2{puck.x - 4.0f, puck.y - 4.0f}, 4.0f, Fade(WHITE, 0.5f));

    drawScoreboard(game);
    drawOverlay(game);
}

void Renderer::drawRink() const {
    float goalTop = (cfg::HEIGHT - cfg::GOAL_HEIGHT) / 2.0f;

    DrawLineEx(Vector2{cfg::WIDTH / 2.0f, 0}, Vector2{cfg::WIDTH / 2.0f, (float)cfg::HEIGHT}, 4.0f, LINE_BLUE);
    DrawCircleLines(cfg::WIDTH / 2, cfg::HEIGHT / 2, 70.0f, LINE_BLUE);
    DrawCircleV(Vector2{cfg::WIDTH / 2.0f, cfg::HEIGHT / 2.0f}, 6.0f, LINE_BLUE);

    // The goal openings, drawn on the edge of the table.
    DrawRectangle(0, (int)goalTop, 12, (int)cfg::GOAL_HEIGHT, PLAYER_COLOR);
    DrawRectangle(cfg::WIDTH - 12, (int)goalTop, 12, (int)cfg::GOAL_HEIGHT, AI_COLOR);

    DrawRectangleLinesEx(Rectangle{0, 0, (float)cfg::WIDTH, (float)cfg::HEIGHT}, 6.0f, Color{60, 80, 110, 255});
}

void Renderer::drawScoreboard(const Game& game) const {
    char text[64];
    snprintf(text, sizeof(text), "YOU  %d : %d  CPU", game.playerScore(), game.aiScore());
    drawCentered(text, 14, 28, Color{40, 60, 90, 255});
}

void Renderer::drawOverlay(const Game& game) const {
    switch (game.state()) {
        case State::Title: {
            DrawRectangle(0, 0, cfg::WIDTH, cfg::HEIGHT, Fade(BLACK, 0.6f));
            drawCentered("AIR HOCKEY", 110, 64, WHITE);
            drawCentered("Move the mouse to control the blue paddle", 200, 22, RAYWHITE);
            drawCentered("First to 7 goals wins", 230, 22, RAYWHITE);
            const char* names[] = {"", "Easy", "Medium", "Hard"};
            char line[64];
            snprintf(line, sizeof(line), "Press 1 / 2 / 3 to pick difficulty   Current: %s", names[game.difficulty()]);
            drawCentered(line, 290, 22, Color{255, 220, 120, 255});
            drawCentered("Press SPACE to start", 350, 28, WHITE);
            break;
        }
        case State::GoalPause:
            drawCentered("GOAL!", 200, 72, game.lastGoalWasPlayers() ? PLAYER_COLOR : AI_COLOR);
            break;
        case State::GameOver: {
            DrawRectangle(0, 0, cfg::WIDTH, cfg::HEIGHT, Fade(BLACK, 0.6f));
            drawCentered(game.playerWon() ? "YOU WIN!" : "CPU WINS", 150, 72, WHITE);
            char line[64];
            snprintf(line, sizeof(line), "Final score  %d : %d", game.playerScore(), game.aiScore());
            drawCentered(line, 250, 28, RAYWHITE);
            drawCentered("Press SPACE to play again", 320, 24, Color{255, 220, 120, 255});
            break;
        }
        case State::Playing:
            break;
    }
}
