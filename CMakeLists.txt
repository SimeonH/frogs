cmake_minimum_required(VERSION 3.16)
project(FrogsOfWarII C)

set(CMAKE_C_STANDARD 99)

# SDL2 — modern SDL2 provides SDL2::SDL2 CMake targets
find_package(SDL2 REQUIRED)

# SDL2_mixer — try CMake config first, fall back to pkg-config
find_package(SDL2_mixer QUIET)
if(NOT SDL2_mixer_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer>=2.0.0)
endif()

set(SOURCES
    src/chardisp.c
    src/ceo.c
    src/frogs.c
    src/helm.c
    src/magi.c
    src/player.c
    src/pond.c
    src/racket.c
    src/room.c
    src/sprite.c
    src/tank.c
    src/util.c
)

add_executable(frogs ${SOURCES})

target_compile_definitions(frogs PRIVATE PKGDATADIR=".")

# SDL2 link
if(TARGET SDL2::SDL2)
    target_link_libraries(frogs PRIVATE SDL2::SDL2)
    if(WIN32 AND TARGET SDL2::SDL2main)
        target_link_libraries(frogs PRIVATE SDL2::SDL2main)
    endif()
else()
    target_include_directories(frogs PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(frogs PRIVATE ${SDL2_LIBRARIES})
endif()

# SDL2_mixer link
if(TARGET SDL2_mixer::SDL2_mixer)
    target_link_libraries(frogs PRIVATE SDL2_mixer::SDL2_mixer)
else()
    target_include_directories(frogs PRIVATE ${SDL2_MIXER_INCLUDE_DIRS})
    target_link_libraries(frogs PRIVATE ${SDL2_MIXER_LIBRARIES})
    if(SDL2_MIXER_LIBRARY_DIRS)
        target_link_directories(frogs PRIVATE ${SDL2_MIXER_LIBRARY_DIRS})
    endif()
endif()

# Windows: Winsock2 for networking in pond.c
if(WIN32)
    target_link_libraries(frogs PRIVATE ws2_32)
endif()
