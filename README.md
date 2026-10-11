> **Bob's Game — Continued.** The base 1:1 port is virtually complete — totally stable and playable.
> Now we continue the work of completing the story, systems, features, and content of Bob's Game however we see fit.
> Forked from [realtechnick/bobsgame-modern](https://github.com/realtechnick/bobsgame-modern) at tag `preservation/1.0` (commit `e4c99da`).
> The upstream repo stays a pristine 1:1 preservation of Robert Pelloni's recovered source — all new content and design lives here.
> **Authorship:** everything by Bob is Bob's. New dialogue, systems, and content are Nick's — never presented as Bob-authored.

# Bob's Game — Continued

The completion build of **Robert Pelloni's** cult DS homebrew RPG *Bob's Game* (2003-2009), recovered from the original C++ source and ported to modern SDL3.

**Status (2026-10-10):** The base port is virtually complete — stable, playable, and running at a rock-solid 120Hz on Apple Silicon. Demo 2 has been completed end-to-end. Work is now focused on finishing what Bob started: completing the story, systems, features, and content.

## About

Bob's Game is a 2D RPG/life-sim by Robert Pelloni. Pelloni passed away, and this may be among the last surviving copies of the source. The preservation port ([bobsgame-modern](https://github.com/realtechnick/bobsgame-modern)) restored his code 1:1 — and now this repo continues where he left off.

**Original source:** `bobsgamed/okClassic` (96 .cpp, 101 .h files)
**Supplemental data:** `bobsgame/BobsGameOnline` v8830 (257 maps)

## What's Working

**Core Game**
- Title screen through town exploration, fully playable
- Demo 2 completed end-to-end (2026-10-07)
- Intro and "MOVING DAY" sequence run correctly
- Town, suburb, and school fully traversable
- Yuu's house interiors with correct brightness
- Dialogue portraits and drop shadows render correctly
- 12 NPC dialogues written and approved

**Rendering**
- Native 120Hz vsync-driven pacing (rock-solid, no stutter)
- Adaptive frame pacing (correct on 60Hz and 120Hz displays)
- All vehicle sprites at sharp 1.0x integer scale
- 640x480 internal render, 1280x960 window, nearest-neighbor

**GameToy Minigames**
- Tetrid, Ping, and Ramio all playable
- Easy-mode win thresholds tuned for faster playtesting

**Audio**
- Music plays and loops, sound effects and ambience work
- Requires SDL3_mixer (see Building)

## Building

### Requirements
- CMake 3.16+
- C++11 compiler (GCC, Clang, MSVC)
- SDL3 (3.2.10+)
- SDL3_ttf (3.2.2+)
- SDL3_mixer (for audio)
- OpenGL

### macOS
```bash
brew install cmake sdl3 sdl3_ttf sdl3_mixer
git clone https://github.com/realtechnick/bobsgame-continued.git
cd bobsgame-continued
mkdir build && cd build
cmake -DBOBSGAME_ENABLE_AUDIO=ON ..
make -j$(sysctl -n hw.ncpu)
```

The game needs the original `data/` assets (not in git). Place the `data/` folder in `build/` alongside the binary, then:
```bash
./bobsgame
```

### Linux
```bash
mkdir build && cd build
cmake -DBOBSGAME_ENABLE_AUDIO=ON ..
make -j$(nproc)
./bobsgame
```

**Note:** Keep the window at 640x480. The game's camera, sprite, and GameToy math is tuned for it — use OS display scaling for a larger picture.

## Project Structure

```
src/                    # C++ source (Bob's original, plus new content)
  engine/               # Core engine (rendering, game loop, audio)
  gamelogic/            # Game logic (maps, NPCs, dialogue)
  minigame/             # Minigames (tetrid, ramio, ping, etc.)
  shaders/              # GLSL shaders
  GL/                   # Bundled GLEW headers
data/                   # Original game data (from okClassic, not in git)
cmake/                  # Build config and SDL3_mixer compatibility shim
```

## Philosophy

**Finish what Bob started.** The preservation work proved Bob's code is meticulously designed — among the most advanced DS homebrew projects. With the base port stable, this repo is where the game gets completed: story, systems, features, and content, guided by what Bob built and where it was heading.

New work is clearly marked — everything by Bob is Bob's, everything new is Nick's. The two are never confused.

See `BOB_PATTERNS.md` for documented patterns from the original source.

## License

Original code by Robert Pelloni (2003-2009). Treated as freeware/abandonware for preservation purposes. This continuation is a fan completion project.
