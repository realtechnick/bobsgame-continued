> **Bob's Game — Continued.** The base 1:1 port is virtually complete — totally stable and playable.
> Now we continue the work of completing the story, systems, features, and content of Bob's Game however we see fit.
> Forked from [realtechnick/bobsgame-modern](https://github.com/realtechnick/bobsgame-modern) at tag `preservation/1.0` (commit `e4c99da`).
> The upstream repo stays a pristine 1:1 preservation of Robert Pelloni's recovered source — all new content and design lives here.
> **Authorship:** everything by Bob is Bob's. New dialogue, systems, and content are Nick's — never presented as Bob-authored.

# Bob's Game — Modern SDL3 Port

A preservation port of **Robert Pelloni's** cult DS homebrew RPG *Bob's Game* (2003-2009), recovered from the original C++ source and migrated from SDL2 to SDL3 for modern systems.

**Status: First playable build** (2026-10-06) — title screen through town exploration works on Apple Silicon Mac.

## About

Bob's Game is a 2D RPG/life-sim by Robert Pelloni. This is preservation work — Pelloni has passed away, and this may be among the last surviving copies of the source. The goal is a 1:1 faithful port: restore Bob's intended behavior, don't reinvent it.

**Original source:** `bobsgamed/okClassic` (96 .cpp, 101 .h files)
**Supplemental data:** `bobsgame/BobsGameOnline` v8830 (257 maps)

## What's Working

**Title Screen**
- Green "bob's game" logo with shine animation, correctly centered
- "a game by one person" tagline and "press the action button" prompt

**Gameplay**
- Town and suburb fully walkable
- School traversable without crashes
- Intro and "MOVING DAY" sequence run correctly
- Yuu's house interiors load with correct brightness
- Dialogue portraits display correctly
- Yuu's drop shadow renders properly

**GameToy**
- Tetrid works correctly (1.0x zoom, 640x480 window)

**Audio**
- Music plays and loops, sound effects work, ambience works
- Requires SDL3_mixer (see Building)

**Performance**
- 60Hz frame logic (correct on any display refresh rate)
- ~119fps on Apple Silicon at 640x480

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
brew install sdl3 sdl3_ttf sdl3_mixer
mkdir build && cd build
cmake -DBOBSGAME_ENABLE_AUDIO=ON ..
make -j$(sysctl -n hw.ncpu)
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
src/                    # C++ source (preserved 1:1 from original)
  engine/               # Core engine (rendering, game loop, audio)
  gamelogic/            # Game logic (maps, NPCs, dialogue)
  minigame/             # Minigames (tetrid, ramio, ping, etc.)
  shaders/              # GLSL shaders
  GL/                   # Bundled GLEW headers
data/                   # Original game data (from okClassic)
data_v8830/             # Supplemental v8830 data (archived, not yet integrated)
cmake/                  # Build config and SDL3_mixer compatibility shim
```

## Philosophy

**Preservation first.** Bob's code is meticulously designed — among the most advanced DS homebrew projects. When something breaks in the port, the answer is almost always "restore Bob's intended behavior," not "redesign the system." Fixes are source-faithful; refactors wait until preservation is complete.

See `BOB_PATTERNS.md` for documented patterns from the source.

## Known Gaps

- `Mix_FadeOutMusic` stops instead of fading; `Mix_HookMusicFinished` unwired
- Clock shows $0 but time/day captions missing during intro
- v8830 data not yet integrated into runtime
- Web build has issues (see web-build/README.md)

## License

Original code by Robert Pelloni (2003-2009). Treated as freeware/abandonware for preservation purposes. This port is for personal/preservation use.
