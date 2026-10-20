#pragma once
#include "Game.h"

// Draws a Game. It only reads the game and never changes it.
class Renderer {
public:
    void draw(const Game& game) const;

private:
    void drawRink() const;
    void drawScoreboard(const Game& game) const;
    void drawOverlay(const Game& game) const;
};
