// Plain C++ tests with no framework and no window. Run with: make test
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "Config.h"
#include "Game.h"
#include "Paddle.h"
#include "Puck.h"

static int failures = 0;
static int checks = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        ++checks;                                                                \
        if (!(cond)) {                                                           \
            ++failures;                                                          \
            std::printf("  FAIL line %d: %s\n", __LINE__, #cond);                \
        }                                                                        \
    } while (0)

static const float DT = 1.0f / 60.0f;

static void run(Game& game, float seconds, const Input& in) {
    int frames = (int)(seconds / DT);
    for (int i = 0; i < frames; ++i) game.update(DT, in);
}

static Game startedGame() {
    Game game;
    Input press;
    press.confirmPressed = true;
    game.update(DT, press);
    return game;
}

static void test_puck_moves_in_its_direction() {
    Puck puck;
    puck.reset(Vector2{100, 100}, Vector2{100, 0});
    puck.update(1.0f);
    CHECK(puck.position().x > 150.0f);
    CHECK(std::fabs(puck.position().y - 100.0f) < 0.001f);
}

static void test_puck_speed_is_capped() {
    Puck puck;
    puck.reset(Vector2{100, 100}, Vector2{5000, 5000});
    puck.update(DT);
    float speed = std::sqrt(puck.velocity().x * puck.velocity().x + puck.velocity().y * puck.velocity().y);
    CHECK(speed <= cfg::PUCK_MAX_SPEED + 0.01f);
}

static void test_puck_slows_down_on_ice() {
    Puck puck;
    puck.reset(Vector2{100, 100}, Vector2{300, 0});
    puck.update(1.0f);
    CHECK(puck.velocity().x < 300.0f);
    CHECK(puck.velocity().x > 0.0f);
}

static void test_bounces_off_top_wall() {
    Game game = startedGame();
    game.debugPlacePuck(Vector2{450, 20}, Vector2{0, -300});
    run(game, 0.2f, Input{});
    CHECK(game.puck().velocity().y > 0.0f);
    CHECK(game.state() == State::Playing);
}

static void test_goal_for_ai_when_puck_enters_left_opening() {
    Game game = startedGame();
    game.debugPlacePuck(Vector2{10, 250}, Vector2{-500, 0});
    run(game, 0.3f, Input{});
    CHECK(game.aiScore() == 1);
    CHECK(game.playerScore() == 0);
    CHECK(game.state() == State::GoalPause);
}

static void test_goal_for_player_when_puck_enters_right_opening() {
    Game game = startedGame();
    game.debugPlacePuck(Vector2{890, 250}, Vector2{500, 0});
    run(game, 0.3f, Input{});
    CHECK(game.playerScore() == 1);
    CHECK(game.lastGoalWasPlayers());
}

static void test_no_goal_when_puck_hits_wall_outside_opening() {
    Game game = startedGame();
    game.debugPlacePuck(Vector2{30, 60}, Vector2{-500, 0});
    run(game, 0.3f, Input{});
    CHECK(game.aiScore() == 0);
    CHECK(game.puck().velocity().x > 0.0f);  // it bounced back
}

static void test_play_resumes_after_goal_pause() {
    Game game = startedGame();
    game.debugPlacePuck(Vector2{10, 250}, Vector2{-500, 0});
    run(game, 0.3f, Input{});
    CHECK(game.state() == State::GoalPause);
    run(game, cfg::GOAL_PAUSE_SECONDS + 0.2f, Input{});
    CHECK(game.state() == State::Playing);
}

static void test_player_paddle_follows_mouse_and_hits_puck() {
    Game game = startedGame();
    Input in;
    in.mouse = Vector2{200, 250};
    run(game, 0.3f, in);
    CHECK(std::fabs(game.player().position().x - 200.0f) < 1.0f);
    CHECK(std::fabs(game.player().position().y - 250.0f) < 1.0f);

    game.debugPlacePuck(Vector2{275, 250}, Vector2{-300, 0});  // rolling toward the paddle
    run(game, 0.4f, in);
    CHECK(game.puck().velocity().x > 0.0f);  // it bounced off
}

static void test_player_paddle_cannot_cross_center_line() {
    Game game = startedGame();
    Input in;
    in.mouse = Vector2{850, 250};
    run(game, 1.0f, in);
    CHECK(game.player().position().x <= cfg::WIDTH / 2.0f - cfg::PADDLE_RADIUS + 0.01f);
}

static void test_ai_chases_puck_on_its_side() {
    Game game = startedGame();
    game.debugPlacePuck(Vector2{700, 80}, Vector2{0, 0});
    float startY = game.ai().position().y;
    run(game, 0.5f, Input{});
    CHECK(game.ai().position().y < startY - 50.0f);
}

static void test_harder_difficulty_moves_ai_faster() {
    Game easy;
    Input pick;
    pick.difficultyKey = 1;
    easy.update(DT, pick);
    Game hard;
    pick.difficultyKey = 3;
    hard.update(DT, pick);

    Input start;
    start.confirmPressed = true;
    easy.update(DT, start);
    hard.update(DT, start);
    easy.debugPlacePuck(Vector2{700, 40}, Vector2{0, 0});
    hard.debugPlacePuck(Vector2{700, 40}, Vector2{0, 0});
    run(easy, 0.3f, Input{});
    run(hard, 0.3f, Input{});
    CHECK(hard.ai().position().y < easy.ai().position().y);
}

static void test_pinned_puck_gets_reserved() {
    Game game = startedGame();
    game.debugPlacePuck(Vector2{884, 484}, Vector2{0, 0});  // resting in a corner, nobody can reach it
    run(game, 3.0f, Input{});
    float dx = game.puck().position().x - 884.0f;
    float dy = game.puck().position().y - 484.0f;
    CHECK(std::sqrt(dx * dx + dy * dy) > 50.0f);
    CHECK(game.state() == State::Playing);
}

static void test_match_ends_at_win_score_and_can_restart() {
    Game game = startedGame();
    for (int i = 0; i < 6000 && game.state() != State::GameOver; ++i) {
        // Aim the freshly served puck at the left goal (only once per serve, so it can actually travel).
        if (game.state() == State::Playing && game.puck().position().x > 100.0f) {
            game.debugPlacePuck(Vector2{10, 250}, Vector2{-500, 0});
        }
        game.update(DT, Input{});
    }
    CHECK(game.state() == State::GameOver);
    CHECK(game.aiScore() == cfg::WIN_SCORE);
    CHECK(!game.playerWon());

    Input press;
    press.confirmPressed = true;
    game.update(DT, press);
    CHECK(game.state() == State::Title);
    game.update(DT, press);
    CHECK(game.state() == State::Playing);
    CHECK(game.aiScore() == 0);
}

int main() {
    struct { const char* name; void (*fn)(); } tests[] = {
        {"puck moves in its direction", test_puck_moves_in_its_direction},
        {"puck speed is capped", test_puck_speed_is_capped},
        {"puck slows down on ice", test_puck_slows_down_on_ice},
        {"bounces off top wall", test_bounces_off_top_wall},
        {"goal for AI in left opening", test_goal_for_ai_when_puck_enters_left_opening},
        {"goal for player in right opening", test_goal_for_player_when_puck_enters_right_opening},
        {"no goal when hitting wall outside opening", test_no_goal_when_puck_hits_wall_outside_opening},
        {"play resumes after goal pause", test_play_resumes_after_goal_pause},
        {"player paddle follows mouse and hits puck", test_player_paddle_follows_mouse_and_hits_puck},
        {"player paddle cannot cross center line", test_player_paddle_cannot_cross_center_line},
        {"AI chases puck on its side", test_ai_chases_puck_on_its_side},
        {"harder difficulty moves AI faster", test_harder_difficulty_moves_ai_faster},
        {"pinned puck gets re-served", test_pinned_puck_gets_reserved},
        {"match ends at win score and can restart", test_match_ends_at_win_score_and_can_restart},
    };
    for (auto& t : tests) {
        int before = failures;
        t.fn();
        std::printf("%s  %s\n", failures == before ? "PASS" : "FAIL", t.name);
    }
    std::printf("\n%d checks, %d failed\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
