cmake_minimum_required(VERSION 3.16)
project(bobsgame)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The original 2009 code uses int->float narrowing in initializer lists.
# Suppress the C++11 narrowing error (was a warning in older compilers).
add_compile_options(-Wno-c++11-narrowing)

# SDL3 - find via CMake, with Homebrew paths for macOS
# On Apple Silicon Macs, Homebrew is in /opt/homebrew
# On Intel Macs and Linux, it's typically /usr/local
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew" "/usr/local")
endif()

find_path(SDL3_INCLUDE_DIR
    NAMES SDL3/SDL.h
    PATHS /opt/homebrew/include /usr/local/include /usr/include
    NO_DEFAULT_PATH
)
find_path(SDL3_INCLUDE_DIR NAMES SDL3/SDL.h)  # fallback to default search

find_library(SDL3_LIBRARY
    NAMES SDL3
    PATHS /opt/homebrew/lib /usr/local/lib /usr/lib
    NO_DEFAULT_PATH
)
find_library(SDL3_LIBRARY NAMES SDL3)  # fallback

if(NOT SDL3_INCLUDE_DIR)
    message(FATAL_ERROR "SDL3 headers not found. Install SDL3:\n"
        "  macOS: brew install sdl3\n"
        "  Ubuntu/Debian: apt install libsdl3-dev (if available) or build from source\n"
        "  https://github.com/libsdl-org/SDL/releases")
endif()

if(NOT SDL3_LIBRARY)
    message(FATAL_ERROR "SDL3 library not found. Install SDL3 (see above).")
endif()

message(STATUS "SDL3 include: ${SDL3_INCLUDE_DIR}")
message(STATUS "SDL3 library: ${SDL3_LIBRARY}")

# SDL3_ttf - same approach
find_path(SDL3_TTF_INCLUDE_DIR
    NAMES SDL3_ttf/SDL_ttf.h
    PATHS /opt/homebrew/include /usr/local/include /usr/include
    NO_DEFAULT_PATH
)
find_path(SDL3_TTF_INCLUDE_DIR NAMES SDL3_ttf/SDL_ttf.h)

find_library(SDL3_TTF_LIBRARY
    NAMES SDL3_ttf
    PATHS /opt/homebrew/lib /usr/local/lib /usr/lib
    NO_DEFAULT_PATH
)
find_library(SDL3_TTF_LIBRARY NAMES SDL3_ttf)

if(SDL3_TTF_INCLUDE_DIR AND SDL3_TTF_LIBRARY)
    message(STATUS "SDL3_ttf include: ${SDL3_TTF_INCLUDE_DIR}")
    message(STATUS "SDL3_ttf library: ${SDL3_TTF_LIBRARY}")
else()
    message(WARNING "SDL3_ttf not found. Install it:\n"
        "  macOS: brew install sdl3_ttf")
endif()

# All source files
set(SOURCE_FILES
    src/engine/game.cpp
    src/engine/game/action.cpp
    src/engine/game/caption.cpp
    src/engine/game/clock.cpp
    src/engine/game/item.cpp
    src/engine/game/letterbox.cpp
    src/engine/game/map.cpp
    src/engine/game/music.cpp
    src/engine/game/npc.cpp
    src/engine/game/npccar.cpp
    src/engine/game/npcfuncs.cpp
    src/engine/game/npcrand.cpp
    src/engine/game/player.cpp
    src/engine/game/save.cpp
    src/engine/game/start.cpp
    src/engine/game/txt.cpp
    src/engine/game/txtanswer.cpp
    src/engine/game/txtkeyboard.cpp
    src/engine/main/controls.cpp
    src/engine/main/debug.cpp
    src/engine/main/error.cpp
    src/engine/main/globals.cpp
    src/engine/main/misc.cpp
    src/engine/main/render.cpp
    src/engine/main/sound.cpp
    src/engine/main/sprites.cpp
    src/engine/main/tilemaps.cpp
    src/gamelogic/changemap.cpp
    src/gamelogic/city/apartment.cpp
    src/gamelogic/city/bank.cpp
    src/gamelogic/city/casino.cpp
    src/gamelogic/city/city.cpp
    src/gamelogic/city/cityhall.cpp
    src/gamelogic/city/deli.cpp
    src/gamelogic/city/elevatedlifeplace.cpp
    src/gamelogic/city/fashionstore.cpp
    src/gamelogic/city/firestation.cpp
    src/gamelogic/city/groovyclub.cpp
    src/gamelogic/city/hospital.cpp
    src/gamelogic/city/hotel.cpp
    src/gamelogic/city/hourlymotel.cpp
    src/gamelogic/city/laundromat.cpp
    src/gamelogic/city/museum.cpp
    src/gamelogic/city/office.cpp
    src/gamelogic/city/partystore.cpp
    src/gamelogic/city/pawnshop.cpp
    src/gamelogic/city/policestation.cpp
    src/gamelogic/city/poolhall.cpp
    src/gamelogic/city/restaurant.cpp
    src/gamelogic/city/stadium.cpp
    src/gamelogic/city/thecafe.cpp
    src/gamelogic/intro/introtown.cpp
    src/gamelogic/intro/introyuushouse.cpp
    src/gamelogic/other/castroom.cpp
    src/gamelogic/other/japanhouse.cpp
    src/gamelogic/other/meetingroom.cpp
    src/gamelogic/other/minigame.cpp
    src/gamelogic/title/demoend.cpp
    src/gamelogic/title/license.cpp
    src/gamelogic/title/titlescreen.cpp
    src/gamelogic/title/warning.cpp
    src/gamelogic/town/arcade.cpp
    src/gamelogic/town/beautysalon.cpp
    src/gamelogic/town/bookstore.cpp
    src/gamelogic/town/church.cpp
    src/gamelogic/town/clubhouse.cpp
    src/gamelogic/town/coffeeshop.cpp
    src/gamelogic/town/crazyladyhouse.cpp
    src/gamelogic/town/departmentstore.cpp
    src/gamelogic/town/doctorsoffice.cpp
    src/gamelogic/town/electronicsstore.cpp
    src/gamelogic/town/friendshouse.cpp
    src/gamelogic/town/gamestore.cpp
    src/gamelogic/town/gasstation.cpp
    src/gamelogic/town/generichouses.cpp
    src/gamelogic/town/grocerystore.cpp
    src/gamelogic/town/mansion.cpp
    src/gamelogic/town/movietheater.cpp
    src/gamelogic/town/petstore.cpp
    src/gamelogic/town/pizzaplace.cpp
    src/gamelogic/town/playground.cpp
    src/gamelogic/town/recordstore.cpp
    src/gamelogic/town/school.cpp
    src/gamelogic/town/tacoburger.cpp
    src/gamelogic/town/toolshed.cpp
    src/gamelogic/town/town.cpp
    src/gamelogic/town/videostore.cpp
    src/gamelogic/town/yuushouse.cpp
    src/main.cpp
    src/glew.c
    src/minigame/microgame.cpp
    src/minigame/ping.cpp
    src/minigame/platformexplore.cpp
    src/minigame/platformjump.cpp
    src/minigame/ramio.cpp
    src/minigame/shootergame.cpp
    src/minigame/tetrid.cpp
)

# PORT: native macOS vsync (macos_vsync.mm, Objective-C++). Apple only.
if(APPLE)
    enable_language(OBJCXX)
    list(APPEND SOURCE_FILES src/engine/main/macos_vsync.mm)
endif()

add_executable(bobsgame ${SOURCE_FILES})

target_include_directories(bobsgame PRIVATE
    ${SDL3_INCLUDE_DIR}
    ${SDL3_TTF_INCLUDE_DIR}
    src
)

target_link_libraries(bobsgame
    ${SDL3_LIBRARY}
    ${SDL3_TTF_LIBRARY}
)

# Copy shaders where the game expects them (loads from ./shaders/ relative to CWD)
add_custom_command(TARGET bobsgame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/shaders $<TARGET_FILE_DIR:bobsgame>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/shaders ${CMAKE_SOURCE_DIR}/shaders
    COMMENT "Copying shaders")

# OpenGL
if(APPLE)
    find_library(OPENGL_LIBRARY OpenGL)
    target_link_libraries(bobsgame ${OPENGL_LIBRARY})
    # PORT: Cocoa framework for macos_vsync.mm (NSOpenGLContext).
    find_library(COCOA_LIBRARY Cocoa)
    target_link_libraries(bobsgame ${COCOA_LIBRARY})
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(bobsgame OpenGL::GL)
endif()

# Audio support (SDL3_mixer)
# Default OFF: SDL3_mixer doesn't build against SDL 3.2.10 (needs 3.4+ APIs)
# When OFF, uses the stub header in cmake/stubs/ (silent, no audio)
# When ON, tries to find system SDL3_mixer
option(BOBSGAME_ENABLE_AUDIO "Enable audio via SDL3_mixer" OFF)

if(BOBSGAME_ENABLE_AUDIO)
    # SDL3_mixer's CMake package requires the SDL3::Headers target, which is
    # only created when SDL3 itself is located via find_package (we locate SDL3
    # manually with find_path/find_library above, e.g. Homebrew layouts).
    find_package(SDL3 QUIET)
    if(NOT TARGET SDL3::Headers)
        add_library(SDL3::Headers INTERFACE IMPORTED)
        if(SDL3_INCLUDE_DIR)
            set_target_properties(SDL3::Headers PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${SDL3_INCLUDE_DIR}")
        endif()
    endif()
    find_package(SDL3_mixer QUIET)
    if(SDL3_mixer_FOUND)
        # Use compatibility shim (maps SDL2_mixer API to SDL3_mixer 3.x)
        target_sources(bobsgame PRIVATE ${CMAKE_SOURCE_DIR}/cmake/compat/sdl3_mixer_compat.c)
        target_include_directories(bobsgame PRIVATE ${CMAKE_SOURCE_DIR}/cmake/compat)
        target_link_libraries(bobsgame SDL3_mixer::SDL3_mixer)
        message(STATUS "Audio: Using SDL3_mixer with compatibility shim")
    else()
        message(WARNING "Audio: SDL3_mixer not found, falling back to stub (silent)")
        target_include_directories(bobsgame PRIVATE ${CMAKE_SOURCE_DIR}/cmake/stubs)
    endif()
else()
    # Use stub header (silent audio)
    target_include_directories(bobsgame PRIVATE ${CMAKE_SOURCE_DIR}/cmake/stubs)
    message(STATUS "Audio: Disabled (using stub, game will be silent)")
endif()

# Note: SDL3_ttf is required and linked above.
