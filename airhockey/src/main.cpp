#include "raylib.h"
#include "Config.h"
#include "Game.h"
#include "Renderer.h"

int main() {
    InitWindow(cfg::WIDTH, cfg::HEIGHT, "Air Hockey");
    SetTargetFPS(60);
    HideCursor();

    Game game;
    Renderer renderer;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.05f) dt = 0.05f;  // if the window freezes for a moment, don't let the puck teleport

        Input input;
        input.mouse = GetMousePosition();
        input.confirmPressed = IsKeyPressed(KEY_SPACE);
        if (IsKeyPressed(KEY_ONE)) input.difficultyKey = 1;
        if (IsKeyPressed(KEY_TWO)) input.difficultyKey = 2;
        if (IsKeyPressed(KEY_THREE)) input.difficultyKey = 3;

        game.update(dt, input);

        BeginDrawing();
        renderer.draw(game);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
