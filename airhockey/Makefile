CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wno-missing-field-initializers -O2
RAYLIB_CFLAGS ?= $(shell pkg-config --cflags raylib)
RAYLIB_LIBS ?= $(shell pkg-config --libs raylib)

LOGIC = src/Puck.cpp src/Paddle.cpp src/Game.cpp
GAME  = $(LOGIC) src/Renderer.cpp src/main.cpp

airhockey: $(GAME) $(wildcard src/*.h)
	$(CXX) $(CXXFLAGS) $(RAYLIB_CFLAGS) -Isrc $(GAME) -o $@ $(RAYLIB_LIBS)

run_tests: $(LOGIC) tests/test_game.cpp $(wildcard src/*.h)
	$(CXX) $(CXXFLAGS) $(RAYLIB_CFLAGS) -Isrc $(LOGIC) tests/test_game.cpp -o $@

.PHONY: play test clean
play: airhockey
	./airhockey

test: run_tests
	./run_tests

clean:
	rm -f airhockey run_tests
