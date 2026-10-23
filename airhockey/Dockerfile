# Builds Air Hockey's game logic and runs its tests inside a container.
# The game window needs a screen, so the container runs the headless tests only.
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Compiler, build tools, and the libraries raylib needs to compile
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential cmake git ca-certificates pkg-config \
        libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev \
    && rm -rf /var/lib/apt/lists/*

# Build and install raylib 5.5 from source
RUN git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git /tmp/raylib \
    && cmake -S /tmp/raylib -B /tmp/raylib/build -DBUILD_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=Release \
    && cmake --build /tmp/raylib/build -j 2 \
    && cmake --install /tmp/raylib/build \
    && ldconfig \
    && rm -rf /tmp/raylib

# Copy the project in and compile the tests
WORKDIR /app
COPY . .
RUN make run_tests

# Running the container runs the tests
CMD ["./run_tests"]
